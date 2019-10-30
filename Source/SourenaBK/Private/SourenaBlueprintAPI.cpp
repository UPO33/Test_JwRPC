#include "SourenaBlueprintAPI.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "SourenaBK.h"
#include "SourenaErrors.h"
#include "TimerManager.h"


FRpcError GRpcErr_NullConnection{ -1, TEXT("Null Connection") };


#define BEGIN_REFRENCE_CHECK_CUSTOM(ref) \
	if (!ref)\
	{\
		this->CallNextFrame([=]() { onError.ExecuteIfBound(GRpcErr_NullConnection); });\
		return; \
	}\


#define BEGIN_REFRENCE_CHECK	BEGIN_REFRENCE_CHECK_CUSTOM(this->IsRefrenceValid()) 






#define CALL_BASE_EMPTY(name)\
	BEGIN_REFRENCE_CHECK;\
	if (this->Refrence)\
	{\
		this->Refrence->##name##(uih, [=]() {\
			onSuccess.ExecuteIfBound();\
			}, [=](FRpcError & error) {\
				onError.ExecuteIfBound(error);\
			});\
	}\



#define CALL_BASE_EMPTY_ZeroParam(name)\
	BEGIN_REFRENCE_CHECK;\
	if (this->Refrence)\
	{\
		this->Refrence->##name##([=]() {\
			onSuccess.ExecuteIfBound();\
			}, [=](FRpcError & error) {\
				onError.ExecuteIfBound(error);\
			});\
	}\




#define CALL_BASE_EMPTY_OneParam(name, param0)\
	BEGIN_REFRENCE_CHECK;\
	if (this->Refrence)\
	{\
		this->Refrence->##name##(param0, [=]() {\
			onSuccess.ExecuteIfBound();\
			}, [=](FRpcError & error) {\
				onError.ExecuteIfBound(error);\
			});\
	}\



#define CALL_BASE_EMPTY_TwoParam(name, param0, param1)\
	BEGIN_REFRENCE_CHECK;\
	if (this->Refrence)\
	{\
		this->Refrence->##name##(param0, param1, [=]() {\
			onSuccess.ExecuteIfBound();\
			}, [=](FRpcError & error) {\
				onError.ExecuteIfBound(error);\
			});\
	}\



USourenaGameServer* USourenaGameServer::Connect(const FString& url, FSourenaEmptyDD onConnect, FSourenaConnectErrorDD onError)
{
	USourenaGameServer* pObj = NewObject<USourenaGameServer>();
	pObj->Refrence = FSourenaGS::Create();

	pObj->Refrence->Connect(url, [=]() {
		onConnect.ExecuteIfBound();
	}, [=](const FString& error) {
		onError.ExecuteIfBound(error);
	});

	return pObj;
}

void USourenaGameServer::Register(const FGSRegisterParams& params, FGSRegisterResultDD onResult, FSourenaFailDD onError)
{
	check(false);
}

void USourenaGameServer::PlayerJoin(const FString& ticket, FGSPlayerJoinResultDD onResult, FSourenaFailDD onError)
{
	check(false);
}

void USourenaGameServer::Unregister(FSourenaEmptyDD onResult, FSourenaFailDD onError)
{
	check(false);
}




const FGSRegisterResult& USourenaGameServer::GetRegisterResult() const
{
	return this->Refrence->RegisterResult;
}

USourenaGameClient* USourenaGameClient::Connect(const FString& url, FSourenaEmptyDD onConnect, FSourenaConnectErrorDD onError)
{
	USourenaGameClient* pObj = NewObject<USourenaGameClient>();
	pObj->Refrence = FSourenaGC::Create();
	pObj->BindDelegates();

	pObj->Refrence->Connect(url, [=]() {
		onConnect.ExecuteIfBound();
	}, [=](const FString& error) {
		onError.ExecuteIfBound(error);
	});

	return pObj;

}

void USourenaGameClient::BindDelegates()
{
	check(this->Refrence);


	this->Refrence->OnRelationUpdate.AddUObject(this, &USourenaGameClient::OnNotify_RelationUpdate);
	this->Refrence->OnPrivateMessage.AddUObject(this, &USourenaGameClient::OnNotify_PrivateMessage);

	this->Refrence->OnFriendOnline.AddUObject(this, &USourenaGameClient::OnNotify_FriendOnline);
	this->Refrence->OnFriendOffline.AddUObject(this, &USourenaGameClient::OnNotify_FriendOffline);


	this->Refrence->OnDebugMessage.AddUObject(this, &USourenaGameClient::OnNotify_DebugMessage);


	this->Refrence->OnPartyMessage.AddUObject(this, &USourenaGameClient::OnNotify_PartyMessage);
	this->Refrence->OnPartyInvite.AddUObject(this, &USourenaGameClient::OnNotify_PartyInvite);
	this->Refrence->OnPartyUserLeft.AddUObject(this, &USourenaGameClient::OnNotify_PartyUserLeft);
	this->Refrence->OnPartyUserJoined.AddUObject(this, &USourenaGameClient::OnNotify_PartyUserJoined);
	this->Refrence->OnPartyUserReady.AddUObject(this, &USourenaGameClient::OnNotify_PartyUserReady);
	this->Refrence->OnPartyPublicDataUpdate.AddUObject(this, &USourenaGameClient::OnNotify_PartyPublicDataUpdate);
	this->Refrence->OnPartyPrivateDataUpdate.AddUObject(this, &USourenaGameClient::OnNotify_PartyPrivateDataUpdate);
}

void USourenaGameClient::LoginByToken(const FString& sessionToken, FGCLoginResultDD onResult, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;

	this->Refrence->LoginByToken(sessionToken, [=](FGCLoginResult & result) {
		onResult.ExecuteIfBound(result);
	}, [=](FRpcError & error) {
		onError.ExecuteIfBound(error);
	});
}

void USourenaGameClient::LoginEmailPass(const FString& email, const FString& password, FGCLoginResultDD onResult, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;

	this->Refrence->LoginEmailPass(email, password, [=](FGCLoginResult& result) {
		onResult.ExecuteIfBound(result);
	}, [=](FRpcError& error) {
		onError.ExecuteIfBound(error);
	});
}

	
void USourenaGameClient::Matchmake(const FGCMatchmakeParams& params, FGCMatchmakeResultDD onResult, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;

	this->Refrence->Matchmake(params, [=](FGCMatchmakeResult& result) {
		onResult.ExecuteIfBound(result);
	}, [=](FRpcError& error) {
		onError.ExecuteIfBound(error);
	});
}

void USourenaGameClient::Logout(FSourenaEmptyDD onResult, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK

	this->Refrence->Logout([=]() {
		onResult.ExecuteIfBound();
	}, [=](FRpcError& error) {
		onError.ExecuteIfBound(error);
	});
}



void USourenaGameClient::RequestFriendship(const FString& uih, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY(RequestFriendship);
}

void USourenaGameClient::CancelPendingFriendship(const FString& uih, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY(CancelPendingFriendship);
}

void USourenaGameClient::AcceptFriendship(const FString& uih, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY(AcceptFriendship)
}

void USourenaGameClient::DeclineFriendship(const FString& uih, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY(DeclineFriendship);
}

void USourenaGameClient::RelationBlockUser(const FString& uih, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY(RelationBlockUser);
}

void USourenaGameClient::RelationUnblockUser(const FString& uih, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY(RelationUnblockUser);
}

void USourenaGameClient::GetRelationships(FGCRelationshipsResultDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;

	this->Refrence->GetRelationships([=](TArray<FGCRelationshipInfo>& result) {
		onSuccess.ExecuteIfBound(result);
		}, [=](FRpcError & error) {
			onError.ExecuteIfBound(error);
	});
}

void USourenaGameClient::FindUserLike(const FString like, FGCFindUserLikeResultsDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;

	this->Refrence->FindUserLike(like, [=](TArray<FGCUserFindInfo>& result) {
		onSuccess.ExecuteIfBound(result);
		}, [=](FRpcError& error) {
			onError.ExecuteIfBound(error);
		});
}

void USourenaGameClient::FriendSendPrivateChat(const FString& friendUIH, const FString& message, FGCFriendSendPrivateChatResultDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;

	this->Refrence->FriendSendPrivateChat(friendUIH, message, [=](FGCPrivateMessage& result) {
			onSuccess.ExecuteIfBound(result);
	}, [=](FRpcError& error) {
			onError.ExecuteIfBound(error);
	});
}

void USourenaGameClient::FriendGetAllPrivateChats(const FString& friendUIH, FGCFriendGetAllPrivateChatsResultsDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;
	this->Refrence->FriendGetAllPrivateChats(friendUIH, [=](TArray<FGCPrivateMessage> & result) {
		onSuccess.ExecuteIfBound(result);
		}, [=](FRpcError & error) {
			onError.ExecuteIfBound(error);
		});
}



void USourenaGameClient::PartyCreate(const FString& partyClass, FGCPartyGetInfoDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;
	this->Refrence->PartyCreate(partyClass, [=](const FSourenaPartyInfo & result) {
		onSuccess.ExecuteIfBound(result);
		}, [=](FRpcError & error) {
			onError.ExecuteIfBound(error);
	});
}

void USourenaGameClient::PartyLeave(FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY_ZeroParam(PartyLeave);
}

void USourenaGameClient::PartyInvite(const FString& guest, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY_OneParam(PartyInvite, guest);
}

void USourenaGameClient::PartyJoin(const FString& ticket, FGCPartyGetInfoDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;
	this->Refrence->PartyJoin(ticket, [=](const FSourenaPartyInfo & result) {
		onSuccess.ExecuteIfBound(result);
		}, [=](FRpcError & error) {
			onError.ExecuteIfBound(error);
	});
}

void USourenaGameClient::PartyGetInfo(FGCPartyGetInfoDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;
	this->Refrence->PartyGetInfo([=](const FSourenaPartyInfo& result) {
			onSuccess.ExecuteIfBound(result);
	}, [=](FRpcError & error) {
			onError.ExecuteIfBound(error);
	});
}

void USourenaGameClient::PartySetPublicData(const FString& key, const FString& value, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY_TwoParam(PartySetPublicData, key, value);
}

void USourenaGameClient::PartySetPrivateData(const FString& key, const FString& value, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY_TwoParam(PartySetPrivateData, key, value);
}

void USourenaGameClient::PartySetReady(bool bReady, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY_OneParam(PartySetReady, bReady);
}

void USourenaGameClient::PartySendMessage(const FString& message, FSourenaEmptyDD onSuccess, FSourenaFailDD onError)
{
	CALL_BASE_EMPTY_OneParam(PartySendMessage, message);
}

void USourenaGameClient::GetUserPresence(const FString& user, FGetUserPresenceResultDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK;
	this->Refrence->GetUserPresence(user, [=](const FSourenaUserPresence& result) {
		onSuccess.ExecuteIfBound(result);
		}, [=](FRpcError & error) {
			onError.ExecuteIfBound(error);
		});
}

void USourenaGameClient::OnNotify_DebugMessage(const FString& message)
{
	OnDebugMessage.Broadcast(message);
}

void USourenaGameClient::OnNotify_RelationUpdate(const FGCRelationUpdateNotification& params)
{
	OnRelationUpdate.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PrivateMessage(const FGCPrivateMessage& params)
{
	OnPrivateMessage.Broadcast(this, params);
}


void USourenaGameClient::OnNotify_FriendOnline(const FSourenaFriendOnOffNotification& params)
{
	OnFriendOnline.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_FriendOffline(const FSourenaFriendOnOffNotification& params)
{
	OnFriendOffline.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PartyMessage(const FSourenaPartyMessage& params)
{
	OnPartyMessage.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PartyInvite(const FSourenaPartyInviteNotification& params)
{
	OnPartyInvite.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PartyUserLeft(const FSourenaPartyUserJoinLeaveNotification& params)
{
	OnPartyUserLeft.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PartyUserJoined(const FSourenaPartyUserJoinLeaveNotification& params)
{
	OnPartyUserJoined.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PartyUserReady(const FSourenaPartyUserReadyNotification& params)
{
	OnPartyUserReady.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PartyPublicDataUpdate(const FSourenaPartyDataNotification& params)
{
	OnPartyPublicDataUpdate.Broadcast(this, params);
}

void USourenaGameClient::OnNotify_PartyPrivateDataUpdate(const FSourenaPartyDataNotification& params)
{
	OnPartyPrivateDataUpdate.Broadcast(this, params);
}

const FGCLoginResult& USourenaGameClient::GetLoginResult() const
{
	return this->Refrence->LoginResult;
}



void USourenaAPIBase::GetDataTable(const FString& tableName, FGetDataTableResultDD onSuccess, FSourenaFailDD onError)
{
	BEGIN_REFRENCE_CHECK_CUSTOM(GetBaseAPI());

	GetBaseAPI()->GetDataTable(tableName, [=](FSourenaDataTable& result) {
		onSuccess.ExecuteIfBound(result);
	}, [=](FRpcError& error) {
		onError.ExecuteIfBound(error);
	});
}

TSharedPtr<FSourenaBaseAPI> USourenaGameClientBase::GetBaseAPI() const
{
	return this->Refrence;
}

TSharedPtr<FSourenaBaseAPI> USourenaGameServerBase::GetBaseAPI() const
{
	return this->Refrence;
}


bool USourenaGameClientBase::IsRefrenceValid() const
{
	return Refrence.IsValid();
}

bool USourenaGameServerBase::IsRefrenceValid() const
{
	return Refrence.IsValid();
}

void USourenaGameClientBase::BeginDestroy()
{
	Super::BeginDestroy();

	if (Refrence) {
		Refrence->Close();
		Refrence = nullptr;
	}
}

void USourenaAPIBase::CallNextFrame(TFunction<void()>&& callback)
{
	if (!callback)
		return;

	UWorld* pWorld = this->GetWorld();
	if (!pWorld)
	{
		//#TODO try to use different way
	}

	if (pWorld)
	{
		pWorld->GetTimerManager().SetTimerForNextTick(MoveTemp(callback));
	}
	else
	{
		UE_LOG(SourenaBK, Error, TEXT("now valid world found."));
		callback();
	}
}

TSharedPtr<FSourenaBaseAPI> USourenaAPIBase::GetBaseAPI() const
{
	return nullptr;
}

void USourenaGameServerBase::BeginDestroy()
{
	Super::BeginDestroy();

	if (Refrence) {
		Refrence->Close();
		Refrence = nullptr;
	}
}