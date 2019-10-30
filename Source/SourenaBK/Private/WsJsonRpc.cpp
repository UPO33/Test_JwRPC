#include "WsJsonRpc.h"

#include "SourenaBK.h"

#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Json.h"
#include "JsonUtilities.h"

#include "Base64.h"
#include "JsonObject.h"

//
static TSharedPtr<FJsonValue> GetJSONTimeoutError()
{
	static TSharedPtr<FJsonValue> Instance;
	if (!Instance)
	{
		TSharedPtr<FJsonObject> root = MakeShared<FJsonObject>();
		root->SetNumberField(TEXT("code"), SOURENA_ERROR_CODE_TIMEOUT);
		root->SetStringField(TEXT("message"), TEXT("timeout"));
		Instance = MakeShared<FJsonValueObject>(root);
	}
	return Instance;
}
static TSharedPtr<FJsonValue> GetJSONKillError()
{
	static TSharedPtr<FJsonValue> Instance;
	if (!Instance)
	{
		TSharedPtr<FJsonObject> root = MakeShared<FJsonObject>();
		root->SetNumberField(TEXT("code"), SOURENA_ERROR_CODE_KILLED);
		root->SetStringField(TEXT("message"), TEXT("killed"));
		Instance = MakeShared<FJsonValueObject>(root);
	}
	return Instance;
}

FWsJsonRpc::FWsJsonRpc()
{

}

FWsJsonRpc::~FWsJsonRpc()
{
	KillAll();
}

void FWsJsonRpc::Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError)
{
	FWebSocketsModule& wsModule = FModuleManager::LoadModuleChecked<FWebSocketsModule>(TEXT("WebSockets"));
	//FWebSocketsModule::Get() retuned null on no editor builds
	TSharedRef<IWebSocket> wsc = wsModule.CreateWebSocket(url);

	wsc->OnConnectionError().AddLambda(onError);
	wsc->OnConnected().AddLambda(onSucess);
	//#Note OnMessage must be binned before Connect()
	wsc->OnMessage().AddRaw(this, &FWsJsonRpc::OnMessage);

	this->Connection = wsc;

	wsc->Connect();
}


void FWsJsonRpc::Request(const FString& method, const FString& params, CallbackJSON result /*= nullptr*/, CallbackJSON error /*= nullptr*/)
{
	FString id = GenId();

	FRequest req;
	req.Method = method;
	req.OnResult = result;
	req.OnError = error;
	req.ExpireTime = TimeSinceStart + DefaultTimeout;
	
	//if (FTimerManager* pTimer = TryGetTimerManager())
	//{
	//	pTimer->SetTimer(req.TimerHandle, [this, id]() {
	//		
	//		UE_LOG(SourenaBK, Warning, TEXT("Request id:%s timed out"), *id);
	//
	//		FRequest* pReq = this->Requests.Find(id);
	//		if (pReq)
	//		{
	//			if (pReq->OnError)
	//				pReq->OnError(GetJSONTimeoutError());
	//
	//			pReq->OnError = nullptr;
	//			this->Requests.Remove(id);
	//		}
	//
	//
	//	}, DefaultTimeout, false);
	//}

	const FString finalData = FString::Printf(TEXT(R"({"id":"%s","method":"%s","params":%s})"), *id, *method, *params);
	Connection->Send(finalData);

	UE_LOG(SourenaBK, Warning, TEXT("OutgingData:%s"), *finalData);

	Requests.Add(id, MoveTemp(req));


}



void FWsJsonRpc::Notify(const FString& method, const FString& params)
{
	const FString finalData = FString::Printf(TEXT(R"(({"method":"%s","params":%s})"), *method, *params);
	Connection->Send(finalData);
	UE_LOG(SourenaBK, Warning, TEXT("OutgingData:%s"), *finalData);
}

FString FWsJsonRpc::GenId()
{
	IdCounter = (IdCounter + 1) % 0xFFffFF;
	return FString::FromInt(IdCounter);
	//return BytesToHex((const uint8*)(&IdCounter), sizeof(IdCounter));
}

void FWsJsonRpc::OnMessage(const FString& data)
{
	UE_LOG(SourenaBK, Warning, TEXT("FWsJsonRpc::OnMessage:%s"), *data);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(data);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(SourenaBK, Error, TEXT("failed to deserialize JSON"));
		return;
	}

	if (JsonObject->HasField(TEXT("method"))) //is it request?
	{
		OnRequestRecv(JsonObject);
	}
	else
	{
		FString id = JsonObject->GetStringField("id");
		FRequest* pRequest = Requests.Find(id);
		if (!pRequest)
		{
			UE_LOG(SourenaBK, Error, TEXT("request with id:%s not found"), *id);
			return;
		}
		

		if (JsonObject->HasField(TEXT("error"))) //is it error respond?
		{
			if (pRequest->OnError)
			{
				pRequest->OnError(JsonObject->TryGetField(TEXT("error")));
				pRequest->OnError = nullptr;
			}
		}

		if (JsonObject->HasField(TEXT("result"))) //is it valid respond?
		{
			if (pRequest->OnResult)
			{
				pRequest->OnResult(JsonObject->TryGetField(TEXT("result")));
				pRequest->OnResult = nullptr;
			}
		}

		

		Requests.Remove(id);
	}

}


void FWsJsonRpc::KillAll()
{
	UE_LOG(SourenaBK, Warning, TEXT("FWsJsonRpc::KillAll NumberOfPendingRequests:%d"), Requests.Num());


	for (auto& iterReq : this->Requests)
	{
		if (iterReq.Value.OnError)
		{
			iterReq.Value.OnError(GetJSONKillError());
			iterReq.Value.OnError = nullptr;
		}
	}

	Requests.Empty();
}

void FWsJsonRpc::CheckExpiredRequests()
{
	float elapsed = TimeSinceStart - LastExpireCheckTime;
	if (elapsed < 10)
		return;

	LastExpireCheckTime = TimeSinceStart;

	TArray<FString> keysToBeRemoved;
	for (auto& iterReq : this->Requests)
	{
		if (TimeSinceStart > iterReq.Value.ExpireTime)
		{
			UE_LOG(SourenaBK, Warning, TEXT("request %s timed out."), *iterReq.Key);

			if (iterReq.Value.OnError)
			{
				iterReq.Value.OnError(GetJSONTimeoutError());
				iterReq.Value.OnError = nullptr;
				keysToBeRemoved.Add(iterReq.Key);
			}
		}
	}

	for (const FString& key : keysToBeRemoved)
		Requests.Remove(key);
}

void FWsJsonRpc::Tick(float DeltaTime)
{
	TimeSinceStart += DeltaTime;

	CheckExpiredRequests();
}

void FWsJsonRpc::OnRequestRecv(TSharedPtr<FJsonObject>& root)
{
	static FString STR_method("method");
	static FString STR_params("params");

	FString method = root->GetStringField(STR_method);
	FNotifyCallbackBase* pNotifyCallback = NotificationCallbacks.Find(method);
	if (pNotifyCallback) //found such a notify callback?
	{
		pNotifyCallback->ExecuteIfBound(root->TryGetField(STR_params));
		return;
	}


	FRequestCallbackBase* pRequestCallback = RequestCallbacks.Find(method);
	if (pRequestCallback)
	{
		//#TODO implement request handling
		return;
	}

	UE_LOG(SourenaBK, Error, TEXT("FWsJsonRpc::OnRequestRecv  method %s not found."), *method);

}
