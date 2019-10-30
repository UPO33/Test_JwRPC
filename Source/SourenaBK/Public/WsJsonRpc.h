#pragma once

#include "WebSocketsModule.h"
#include "IWebSocket.h"

#include "Json.h"
#include "JsonUtilities.h"

#include "Base64.h"
#include "Tickable.h"

class FTimerManager;


///////////////////////////////////////////////////////////////
class SOURENABK_API FWsJsonRpc
{
public:
	typedef TFunction<void(TSharedPtr<FJsonValue>)> CallbackJSON;
	typedef FString FMethodName;

	struct FIncomingRequest
	{
	};

	DECLARE_DELEGATE_OneParam(FNotifyCallbackBase, TSharedPtr<FJsonValue> /*params*/);
	DECLARE_DELEGATE_TwoParams(FRequestCallbackBase, TSharedPtr<FJsonValue> /*params*/, TSharedPtr<FIncomingRequest>);

	int IdCounter = 0;
	TSharedPtr<IWebSocket> Connection;
	//default timeout in seconds
	float DefaultTimeout = 60;
	//
	float TimeSinceStart = 0;
	float LastExpireCheckTime = 0;

	struct FRequest
	{
		FString Method;
		CallbackJSON OnError;
		CallbackJSON OnResult;
		float ExpireTime;
	};
	//requests waiting for respond
	TMap<FString, FRequest> Requests;

	FWsJsonRpc();
	~FWsJsonRpc();

	void Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError);

	void Request(const FString& method, const FString& params, CallbackJSON result = nullptr, CallbackJSON error = nullptr);

	/*
	template version that convert error or result JSON to UE4 struct
	*/
	template<class TResultStruct, class TErrorStruct>
	void RequestT(const FString& method, const FString& params, TFunction<void(TResultStruct&)> onResult, TFunction<void(TErrorStruct&)> onError)
	{
		Request(method, params, 
		[onResult](TSharedPtr<FJsonValue> jsResult) {
			TResultStruct resultStruct;
			if (FJsonObjectConverter::JsonObjectToUStruct<TResultStruct>(jsResult->AsObject().ToSharedRef(), &resultStruct))
			{
				if (onResult) onResult(resultStruct);
				return;
			}
		},
		[onError](TSharedPtr<FJsonValue> jsError) {
			TErrorStruct errStruct;
			if (FJsonObjectConverter::JsonObjectToUStruct<TErrorStruct>(jsError->AsObject().ToSharedRef(), &errStruct))
			{
				if (onError) onError(errStruct);
			}
		});
	}
	/*
	template version that converts the result to an array of UE4 struct
	*/
	template<class TResultStruct, class TErrorStruct>
	void RequestTArray(const FString& method, const FString& params, TFunction<void(TArray<TResultStruct>&)> onResult, TFunction<void(TErrorStruct&)> onError)
	{
		Request(method, params,
			[onResult](TSharedPtr<FJsonValue> jsResult) {
				TArray<TResultStruct> resultArray;
				if (FJsonObjectConverter::JsonArrayToUStruct<TResultStruct>(jsResult->AsArray(), &resultArray, 0, 0))
				{
					if (onResult) onResult(resultArray);
					return;
				}
			},
			[onError](TSharedPtr<FJsonValue> jsError) {
				TErrorStruct errStruct;
				if (FJsonObjectConverter::JsonObjectToUStruct<TErrorStruct>(jsError->AsObject().ToSharedRef(), &errStruct))
				{
					if (onError) onError(errStruct);
				}
			});
	}
	//for those who have empty result
	template<class TErrorStruct>
	void RequestErrOnly(const FString& method, const FString& params, TFunction<void()> onResult, TFunction<void(TErrorStruct&)> onError)
	{
		Request(method, params,
			[onResult](TSharedPtr<FJsonValue> jsResult) {
				if (onResult) onResult();
				return;
			},
			[onError](TSharedPtr<FJsonValue> jsError) {
			TErrorStruct errStruct;
			if (FJsonObjectConverter::JsonObjectToUStruct<TErrorStruct>(jsError->AsObject().ToSharedRef(), &errStruct))
			{
				if (onError) onError(errStruct);
			}
		});
	}
	//for those who have custom json result
	template<class TErrorStruct>
	void RequestCustom(const FString& method, const FString& params, TFunction<void(TSharedPtr<FJsonValue>)> onResult, TFunction<void(TErrorStruct&)> onError)
	{
		Request(method, params, onResult,
			[onError](TSharedPtr<FJsonValue> jsError) {
				TErrorStruct errStruct;
				if (FJsonObjectConverter::JsonObjectToUStruct<TErrorStruct>(jsError->AsObject().ToSharedRef(), &errStruct))
				{
					if (onError) onError(errStruct);
				}
			});
	}
	//for those who have FString& result
	template<class TErrorStruct>
	void RequestString(const FString& method, const FString& params, TFunction<void(FString&)> onResult, TFunction<void(TErrorStruct&)> onError)
	{
		Request(method, params,
			[onResult](TSharedPtr<FJsonValue> jsResult) {
				FString str = jsResult->AsString();
				if(onResult)
					onResult(str);
				return;
			},
			[onError](TSharedPtr<FJsonValue> jsError) {
				TErrorStruct errStruct;
				if (FJsonObjectConverter::JsonObjectToUStruct<TErrorStruct>(jsError->AsObject().ToSharedRef(), &errStruct))
				{
					if (onError) onError(errStruct);
				}
			});
	}

	//for those who have TMap<FString,FString>& result
	template<class TErrorStruct>
	void RequestMapString(const FString& method, const FString& params, TFunction<void(TMap<FString,FString>&)> onResult, TFunction<void(TErrorStruct&)> onError)
	{
		Request(method, params,
			[onResult](TSharedPtr<FJsonValue> jsResult) {
				const TSharedPtr<FJsonObject>* pObj = nullptr;
				if (jsResult->TryGetObject(pObj))
				{
					TMap<FString, FString> resultMap;
					const auto& theMap = pObj->Get()->Values;
					for (const auto& mapItem : theMap) 
					{
						resultMap.Add(mapItem.Key, mapItem.Value->AsString());
					}

					if (onResult) 
						onResult(resultMap);
				}
			},
			[onError](TSharedPtr<FJsonValue> jsError) {
				TErrorStruct errStruct;
				if (FJsonObjectConverter::JsonObjectToUStruct<TErrorStruct>(jsError->AsObject().ToSharedRef(), &errStruct))
				{
					if (onError) onError(errStruct);
				}
			});
	}

	//for those who have TMap<FString,TMap<FName,FString>>& result
	template<class TErrorStruct>
	void Request_MapStringNameString(const FString& method, const FString& params, TFunction<void(TMap<FString,TMap<FName,FString>>&)> onResult, TFunction<void(TErrorStruct&)> onError)
	{
		Request(method, params,
			[onResult](TSharedPtr<FJsonValue> jsResult) {
				if (!onResult)
					return;

				TMap<FString,TMap<FName,FString>> finalResult;

				const TSharedPtr<FJsonObject>& paramsAsObj = jsResult->AsObject();
				for (const auto& userPair : paramsAsObj->Values)
				{
					TMap<FName, FString>& dataEntry = finalResult.FindOrAdd(userPair.Key);
					TSharedPtr<FJsonObject> dataRowPtr = userPair.Value->AsObject();
					for (const auto& dataPair : dataRowPtr->Values)
					{
						FName fieldName = FName(*dataPair.Key);
						dataEntry.Add(fieldName, dataPair.Value->AsString());
					}
				}

				onResult(finalResult);
			},
			[onError](TSharedPtr<FJsonValue> jsError) {
				TErrorStruct errStruct;
				if (FJsonObjectConverter::JsonObjectToUStruct<TErrorStruct>(jsError->AsObject().ToSharedRef(), &errStruct))
				{
					if (onError) onError(errStruct);
				}
			});
	}



	//send a notify to server. notify has no respond its just one way.
	void Notify(const FString& method, const FString& params);
	//generates and returns a new id. 
	FString GenId();
	//this is called when we receive data from the server
	void OnMessage(const FString& data);
	//kill all pending requests or any kind of callback who is waiting to be called
	void KillAll();
	void CheckExpiredRequests();
	
	void Tick(float DeltaTime);
	void OnRequestRecv(TSharedPtr<FJsonObject>& root);



	

	
	TMap<FMethodName, FNotifyCallbackBase>	NotificationCallbacks;
	TMap<FMethodName, FRequestCallbackBase>	RequestCallbacks;

};