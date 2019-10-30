#include "SourenaAPI.h"

#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Json.h"
#include "JsonUtilities.h"


#include "SourenaBK.h"
#include "Base64.h"

#include "WsJsonRpc.h"

const FString JSONSTR_EMPTY_OBJECT("{}");
const FString JSONSTR_EMPTY_ARRAY("[]");


FString HelperArrayToJSONString(const TArray<FString>& elements) 
{
	FString ret = TEXT("[");
	ret.Reserve(elements.Num() * 60);
	for (int i = 0; i < elements.Num(); i++)
	{
		if (i != 0)
			ret += ',';

		ret += '"';
		ret += elements[i];
		ret += '"';
	}
	ret += TEXT("]");
	return ret;
}



inline const TCHAR* BoolToStr(bool b) { return b ? TEXT("true") : TEXT("false"); }
inline FString ToJSONString(const FString& str) { return FString::Printf(TEXT("\"%s\""), *str); }



static void ParseDataTableResult(TSharedPtr<FJsonValue> params, FSourenaDataTable& out)
{
	static FString S_tableName("tableName");
	static FString s_properties("properties");

	auto paramsAsObj = params->AsObject();
	out.TableName = FName(*paramsAsObj->GetStringField(S_tableName));
	auto properties = paramsAsObj->GetObjectField(s_properties);
	for (const auto& item : properties->Values)
	{
		out.Properties.Add(FName(*item.Key), item.Value);
	}

}

//////////////////////////////////////////////////////////////////////////
class FSourenaGCImpl : public FSourenaGC, public FTickableGameObject
{
public:
	FWsJsonRpc WsRpc;

	virtual bool IsGameClient() const override { return true; }


	void Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError) override
	{
		WsRpc.Connect(url, onSucess, onError);
	}

	void LoginEmailPass_Main(const FString& identity, const FString& password, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError)
	{
		const TCHAR* pattern = TEXT(R"({"identity":"%s","password":"%s"})");
		WsRpc.RequestT<FGCLoginResult>(TEXT("GCLoginEmailPass"), FString::Printf(pattern, *identity, *password), onSuccess, onError);
	}
	void LoginByToken_Main(const FString& sessionToken, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError)
	{
		const TCHAR* pattern = TEXT("\"%s\"");
		WsRpc.RequestT<FGCLoginResult>(TEXT("GCLoginByToken"), FString::Printf(pattern, *sessionToken), onSuccess, onError);
	}
	void LoginSteam_Main(const FString& ticket, bool bCreate, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError)
	{
		const TCHAR* pattern = TEXT(R"({"ticket":"%s","create":%s})");
		WsRpc.RequestT<FGCLoginResult>(TEXT("GCLoginSteam"), FString::Printf(pattern, *ticket, BoolToStr(bCreate)), onSuccess, onError);
	}
	void LoginEmailPass(const FString& identity, const FString& password, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError) override
	{
		LoginEmailPass_Main(identity, password, [=](FGCLoginResult & loginResult) {
			this->LoginResult = loginResult;
			if (onSuccess) onSuccess(loginResult);
			}, [=](FRpcError & error) {
				if (onError) onError(error);
			});
	}
	void LoginByToken(const FString& sessionToken, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError) override
	{
		LoginByToken_Main(sessionToken, [=](FGCLoginResult & loginResult) {
			this->LoginResult = loginResult;
			if (onSuccess) onSuccess(loginResult);
			}, [=](FRpcError & error) {
				if (onError) onError(error);
			});
	}
	void LoginSteam(const FString& ticket, bool bCreate, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError) override
	{
		LoginSteam_Main(ticket, bCreate, [=](FGCLoginResult & loginResult) {
			this->LoginResult = loginResult;
			if (onSuccess) onSuccess(loginResult);
			}, [=](FRpcError & error) {
				if (onError) onError(error);
			});
	}
	void Matchmake(const FGCMatchmakeParams& params, TFunction<void(FGCMatchmakeResult&)> onSuccess, ErrorCB onError) override
	{
		FString userTags = HelperArrayToJSONString(params.UserTags);

		const TCHAR* pattern = TEXT(R"({"build":"%s","gameMode":"%s", "userTags":%s})");
		WsRpc.RequestT<FGCMatchmakeResult>(TEXT("GCMatchmake"), FString::Printf(pattern, *params.Build, *params.GameMode, *userTags), onSuccess, onError);
	}
	void Close(int code = 1000, const FString& reason = FString()) override {
		if (WsRpc.Connection)
		{
			WsRpc.Connection->Close(code, reason);
			WsRpc.Connection = nullptr;
			WsRpc.IdCounter = 0;
		}
	}



	void RequestFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"uih":"%s"})");
		WsRpc.RequestErrOnly(TEXT("GCRequestFriendship"), FString::Printf(pattern, *uih), onSuccess, onError);
	}
	void CancelPendingFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"uih":"%s"})");
		WsRpc.RequestErrOnly(TEXT("GCCancelPendingFriendship"), FString::Printf(pattern, *uih), onSuccess, onError);
	}
	void AcceptFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"uih":"%s"})");
		WsRpc.RequestErrOnly(TEXT("GCAcceptFriendship"), FString::Printf(pattern, *uih), onSuccess, onError);
	}
	void DeclineFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"uih":"%s"})");
		WsRpc.RequestErrOnly(TEXT("GCDeclineFriendship"), FString::Printf(pattern, *uih), onSuccess, onError);
	}
	void RelationBlockUser(const FString& uih, EmptyCB onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"uih":"%s"})");
		WsRpc.RequestErrOnly(TEXT("GCRelationBlockUser"), FString::Printf(pattern, *uih), onSuccess, onError);
	}
	void RelationUnblockUser(const FString& uih, EmptyCB onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"uih":"%s"})");
		WsRpc.RequestErrOnly(TEXT("GCRelationUnblockUser"), FString::Printf(pattern, *uih), onSuccess, onError);
	}



	void GetRelationships(TFunction<void(TArray<FGCRelationshipInfo>&)> onSuccess, ErrorCB onError) override
	{
		WsRpc.RequestTArray<FGCRelationshipInfo>(TEXT("GCGetRelationships"), TEXT("{}"), onSuccess, onError);
	};


	void FindUserLike(const FString like, TFunction<void(TArray<FGCUserFindInfo>&)> onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"like":"%s"})");
		const FString params = FString::Printf(pattern, *like);
		WsRpc.RequestT<FGCFindUserLikeResult>(TEXT("GCFindUserLike"), params, [onSuccess](FGCFindUserLikeResult & result) {

			//unpack the array
			TArray<FGCUserFindInfo> finalArray;
			finalArray.AddDefaulted(result.Hashes.Num());
			for (int i = 0; i < result.Hashes.Num(); i++)
			{
				finalArray[i].UIH = result.Hashes[i];
				finalArray[i].Username = result.Usernames[i];
				finalArray[i].Email = result.Emails[i];
			}

			if (onSuccess)
				onSuccess(finalArray);

			}, onError);
	}
	void FriendSendPrivateChat(const FString& friendUIH, const FString& message, TFunction<void(FGCPrivateMessage&)> onSuccess, ErrorCB onError) override
	{
		static FString StrMethod = TEXT("GCFriendSendPrivateChat");
		const TCHAR* pattern = TEXT(R"({"receiver":"%s", "message":"%s"})");
		const FString params = FString::Printf(pattern, *friendUIH, *message);
		WsRpc.RequestT<FGCPrivateMessage>(StrMethod, params, onSuccess, onError);
	}
	void FriendGetAllPrivateChats(const FString& friendUIH, TFunction<void(TArray<FGCPrivateMessage>&)> onSuccess, ErrorCB onError) override
	{
		const TCHAR* pattern = TEXT(R"({"other":"%s"})");
		const FString params = FString::Printf(pattern, *friendUIH);
		WsRpc.RequestTArray(TEXT("FriendGetAllPrivateChats"), params, onSuccess, onError);
	}
	void GetDataTable(const FString& tableName, TFunction<void(FSourenaDataTable&)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = TEXT("GCGetDataTable");
		const FString params = FString::Printf(TEXT(R"({"name":"%s"})"), *tableName);
		WsRpc.RequestCustom(SMethod, params, [=](TSharedPtr<FJsonValue> result) {
			
			FSourenaDataTable resultStrut;
			ParseDataTableResult(result, resultStrut);
			if(onSuccess) onSuccess(resultStrut);

		}, onError);
	}


#pragma region party



	void PartyCreate(const FString& partyClass, TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartyCreate";
		const FString params = FString::Printf(TEXT(R"({"partyClass":"%s"})"), *partyClass);
		WsRpc.RequestT(SMethod, params, onSuccess, onError);
	}
	void PartyLeave(EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartyLeave";
		WsRpc.RequestErrOnly(SMethod, JSONSTR_EMPTY_OBJECT, onSuccess, onError);
	}
	void PartyInvite(const FString& guest, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartyInvite";
		const FString params = FString::Printf(TEXT(R"({"guest":"%s"})"), *guest);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}
	void PartyJoin(const FString& ticket, TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartyJoin";
		const FString params = FString::Printf(TEXT(R"({"ticket":"%s"})"), *ticket);
		WsRpc.RequestT(SMethod, params, onSuccess, onError);
	}
	void PartyJoinDirect(const FString& user, TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartyJoinDirect";
		const FString params = FString::Printf(TEXT(R"({"user":"%s"})"), *user);
		WsRpc.RequestT(SMethod, params, onSuccess, onError);
	}
	void PartyGetInfo(TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartyGetInfo";
		WsRpc.RequestT(SMethod, JSONSTR_EMPTY_OBJECT, onSuccess, onError);
	}
	void PartySetPublicData(const FString& key, const FString& value, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartySetPublicData";
		const FString params = FString::Printf(TEXT(R"({"key":"%s", "value":"%s"})"), *key, *value);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}
	void PartySetPrivateData(const FString& key, const FString& value, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartySetPrivateData";
		const FString params = FString::Printf(TEXT(R"({"key":"%s", "value":"%s"})"), *key, *value);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}
	void PartySetReady(bool bReady, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartySetReady";
		const FString params = FString::Printf(TEXT(R"({"ready":%s})"), BoolToStr(bReady));
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}
	void PartySendMessage(const FString& message, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCPartySendMessage";
		const FString params = FString::Printf(TEXT(R"({"message":"%s"})"), *message);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}
#pragma  endregion

#pragma region inventory

	
#pragma endregion



#pragma region UserData

	void GetUserDataReadOnly(const FString& key, StringCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCUserGetDataRO";
		WsRpc.RequestString(SMethod, ToJSONString(key), onSuccess, onError);
	}
	void GetUserDataReadOnlyAll(StringMapCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCUserGetDataROAll";
		WsRpc.RequestMapString(SMethod, JSONSTR_EMPTY_OBJECT, onSuccess, onError);
	}
	void GetUserData(const FString& key, StringCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCUserGetData";
		WsRpc.RequestString(SMethod, ToJSONString(key), onSuccess, onError);
	}
	void GetUserDataAll(StringMapCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCUserGetDataAll";
		WsRpc.RequestMapString(SMethod, JSONSTR_EMPTY_OBJECT, onSuccess, onError);
	}
	void SetUserData(const FString& key, const FString& value, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCUserSetData";
		const FString params = FString::Printf(TEXT(R"({"key":"%s", "value":%s})"), *key, *value);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}

#pragma endregion UserData



	void GetUserPresence(const FString& user, TFunction<void(FSourenaUserPresence&)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GCGetUserPresence";
		const FString params = FString::Printf(TEXT(R"({"user":"%s"})"), *user);
		WsRpc.RequestT(SMethod, params, onSuccess, onError);
	}
	FSourenaGCImpl()
	{

	}
	void Init()
	{
		//#Note theses binds should not be called in constructor

		//bin the notifications
		WsRpc.NotificationCallbacks.Add(TEXT("NotifyPrivateMessage"), FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PrivateMessage));

		//#DEPRECATED
		//WsRpc.NotificationCallbacks.Add(TEXT("ReceivedFriendRequest")	, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_ReceivedFriendRequest));
		//WsRpc.NotificationCallbacks.Add(TEXT("FriendshipAccepted")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_FriendshipAccepted));
		//WsRpc.NotificationCallbacks.Add(TEXT("FriendshipDeclined")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_FriendshipDeclined));
		//WsRpc.NotificationCallbacks.Add(TEXT("RelationBlocked")			, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_RelationBlocked));
		//WsRpc.NotificationCallbacks.Add(TEXT("RelationUnblocked")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_RelationUnblocked));

		WsRpc.NotificationCallbacks.Add(TEXT("NotifyRelationUpdate"), FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_RelationUpdate));
		WsRpc.NotificationCallbacks.Add(TEXT("NotifyFriendOnline")	, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_FriendOnline));
		WsRpc.NotificationCallbacks.Add(TEXT("NotifyFriendOffline")	, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_FriendOffline));


		WsRpc.NotificationCallbacks.Add(TEXT("PartyMessage")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PartyMessage));
		WsRpc.NotificationCallbacks.Add(TEXT("PartyInvite")			, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PartyInvite));
		WsRpc.NotificationCallbacks.Add(TEXT("PartyUserLeft")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PartyUserLeft));
		WsRpc.NotificationCallbacks.Add(TEXT("PartyUserJoined")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PartyUserJoined));
		WsRpc.NotificationCallbacks.Add(TEXT("PartyUserReady")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PartyUserReady));
		WsRpc.NotificationCallbacks.Add(TEXT("PartyPublicData")		, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PartyPublicDataUpdate));
		WsRpc.NotificationCallbacks.Add(TEXT("PartyPrivateData")	, FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_PartyPrivateDataUpdate));


		WsRpc.NotificationCallbacks.Add(TEXT("MatchFailed"), FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_MatchFailed));
		WsRpc.NotificationCallbacks.Add(TEXT("MatchReady"),  FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_MatchReady));

		WsRpc.NotificationCallbacks.Add(TEXT("Debug_Message"), FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGCImpl::OnNotify_DebugMessage));

		//bind the requests
	}
	TSharedRef<FSourenaGC> GetAsShared()
	{
		return StaticCastSharedRef<FSourenaGC>(FSourenaBaseAPI::AsShared());
	}


#define HELPER_PARSE_NOTIFICATION_STRUCT(TheStruct, TheEvent)\
	if (params && params->Type == EJson::Object)\
	{\
		TheStruct paramsStruct;\
		if (FJsonObjectConverter::JsonObjectToUStruct(params->AsObject().ToSharedRef(), &paramsStruct))\
		{\
			TheEvent.Broadcast(paramsStruct);\
			return;\
		}\
	}\
	UE_LOG(SourenaBK, Error, TEXT("OnNotify_XXXX JSON to Object failed"));\


	void OnNotify_RelationUpdate(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FGCRelationUpdateNotification, OnRelationUpdate);
	}
	void OnNotify_FriendOnline(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaFriendOnOffNotification, OnFriendOnline);
	}
	void OnNotify_FriendOffline(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaFriendOnOffNotification, OnFriendOffline);
	}


#pragma region party

	void OnNotify_PartyUserReady(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaPartyUserReadyNotification, OnPartyUserReady);
	}
	void OnNotify_PartyUserJoined(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaPartyUserJoinLeaveNotification, OnPartyUserJoined);
	}
	void OnNotify_PartyUserLeft(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaPartyUserJoinLeaveNotification, OnPartyUserLeft);
	}
	void OnNotify_PartyMessage(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaPartyMessage, OnPartyMessage);
	}
	void OnNotify_PartyInvite(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaPartyInviteNotification, OnPartyInvite);
	}
	void OnNotify_PartyPublicDataUpdate(TSharedPtr<FJsonValue> params) 
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaPartyDataNotification, OnPartyPublicDataUpdate);
	}
	void OnNotify_PartyPrivateDataUpdate(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FSourenaPartyDataNotification, OnPartyPrivateDataUpdate);
	}
#pragma  endregion

	void OnNotify_MatchFailed(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FGCMatchFailNotification, OnMatchFailed);
	}
	void OnNotify_MatchReady(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FGCMatchReadyNotification, OnMatchReady);
	}
	

	void OnNotify_PrivateMessage(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FGCPrivateMessage, OnPrivateMessage);
	}
	void OnNotify_DebugMessage(TSharedPtr<FJsonValue> params)
	{
		FString message;
		if (params && params->TryGetString(message))
		{
			OnDebugMessage.Broadcast(message);
		}

	}

	~FSourenaGCImpl() 
	{
		UE_LOG(SourenaBK, Log, TEXT("~FSourenaGCImpl()"));
		//it seems that WebSockets Module is keeping the reference of IWebsocket so it won't get closed if this object destroys. we do it manually here
		Close();
	}

	bool IsTickableWhenPaused() const override { return true; }
	bool IsTickableInEditor() const override { return true; }
	TStatId GetStatId() const override { return TStatId(); }

	void Tick(float DeltaTime) override
	{
		WsRpc.Tick(DeltaTime);
	}

	

};


//////////////////////////////////////////////////////////////////////////
class FSourenaGSImpl : public FSourenaGS, public FTickableGameObject
{
public:
	FWsJsonRpc WsRpc;

	virtual bool IsGameServer() const { return true; }

	void Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError) override
	{
		WsRpc.Connect(url, onSucess, onError);
	}
	void Register_Main(const FGSRegisterParams& params, TFunction<void(FGSRegisterResult&)> onSuccess, ErrorCB onError)
	{
		const TCHAR* pattern = TEXT(R"({"lobbyToken":"%s","port":%d,"address":"%s"})");
		FString paramsStr = FString::Printf(pattern, *params.LobbyToken, params.Port, *params.Address);
		WsRpc.RequestT<FGSRegisterResult>(TEXT("GSRegister"), paramsStr, onSuccess, onError);
	}
	void Register(const FGSRegisterParams& params, TFunction<void(FGSRegisterResult&)> onSuccess, ErrorCB onError) override
	{
		Register_Main(params, [=](FGSRegisterResult& regRes) {
			this->RegisterResult = regRes;
			if (onSuccess) onSuccess(regRes);
		}, [=](FRpcError& error) {
			if (onError) onError(error);
		});
	}
	void Unregister(EmptyCB onSuccess, ErrorCB onError) override
	{
		WsRpc.RequestErrOnly(TEXT("GSUnregister"), TEXT("{}"), onSuccess, onError);
	}
	void PlayerJoin(const FString& ticket, TFunction<void(FGSPlayerJoinResult&)> onSuccess, ErrorCB onError) override 
	{
		FString paramsStr = FString::Printf(TEXT(R"({"playerTicket":"%s"})"), *ticket);
		WsRpc.RequestT(TEXT("GSPlayerJoin"), paramsStr, onSuccess, onError);
	}
	void PlayerLeave(const FString& ticket, EmptyCB onSuccess, ErrorCB onError) override
	{
		FString paramsStr = FString::Printf(TEXT(R"({"playerTicket":"%s"})"), *ticket);
		WsRpc.RequestErrOnly(TEXT("GSLeavePlayer"), paramsStr, onSuccess, onError);
	}
	void ReportBan(const FString& ticket, const FString& reason, int durationMinutes, EmptyCB onSuccess, ErrorCB onError) override
	{

	}
	void LockLobby(EmptyCB onSuccess, ErrorCB onError) override
	{
		WsRpc.RequestErrOnly(TEXT("GSLockLobby"), TEXT("{}"), onSuccess, onError);
	}

	void GetDataTable(const FString& tableName, TFunction<void(FSourenaDataTable&)> onSuccess, ErrorCB onError) override
	{
		//const TCHAR* pattern = TEXT(R"({"name":"%s"})");
		//const FString params = FString::Printf(pattern, *tableName);
		//WsRpc.RequestT(TEXT("GSGetDataTable"), params, onSuccess, onError);
		
		static FString SMethod = TEXT("GSGetDataTable");
		const FString params = FString::Printf(TEXT(R"({"name":"%s"})"), *tableName);
		WsRpc.RequestCustom(SMethod, params, [=](TSharedPtr<FJsonValue> result) {

			FSourenaDataTable resultStrut;
			ParseDataTableResult(result, resultStrut);
			if(onSuccess) onSuccess(resultStrut);

		}, onError);
	}


#pragma region UserData
	void GetUserDataReadOnly(const FString& user, const FString& key, StringCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUserGetDataRO";
		const FString params = FString::Printf(TEXT(R"({"user":"%s", "key":"%s"})"), *user, *key);
		WsRpc.RequestString(SMethod, params, onSuccess, onError);
	}
	void GetUserDataReadOnlyAll(const FString& user, StringMapCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUserGetDataROAll";
		const FString params = FString::Printf(TEXT(R"({"user":"%s"})"), *user);
		WsRpc.RequestMapString(SMethod, params, onSuccess, onError);
	}

	
	void GetUserData(const FString& user, const FString& key, StringCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUserGetData";
		const FString params = FString::Printf(TEXT(R"({"user":"%s", "key":"%s"})"), *user, *key);
		WsRpc.RequestString(SMethod, params, onSuccess, onError);
	}
	void GetUserDataAll(const FString& user, StringMapCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUserGetDataAll";
		const FString params = FString::Printf(TEXT(R"({"user":"%s"})"), *user);
		WsRpc.RequestMapString(SMethod, params, onSuccess, onError);
	}
	void GetUsersDataReadOnlyAll(const TArray<FString>& users, StringNameStringMapCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUsersGetDataROAll";
		const FString params = FString::Printf(TEXT(R"({"users":%s})"), *HelperArrayToJSONString(users));
		WsRpc.Request_MapStringNameString(SMethod, params, onSuccess, onError);
	}
	void GetUsersDataAll(const TArray<FString>& users, StringNameStringMapCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUsersGetDataAll";
		const FString params = FString::Printf(TEXT(R"({"users":%s})"), *HelperArrayToJSONString(users));
		WsRpc.Request_MapStringNameString(SMethod, params, onSuccess, onError);
	}
	void GetUsersDataAll_v2(const TArray<FString>& users, TFunction<void(TSharedPtr<FJsonObject>)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUsersGetDataAll";
		const FString params = FString::Printf(TEXT(R"({"users":%s})"), *HelperArrayToJSONString(users));
		WsRpc.RequestCustom(SMethod, params, [=](TSharedPtr<FJsonValue> result) {
			
			if (onSuccess) 
				onSuccess(result->AsObject());

		}, onError);
	}
	void GetUsersDataReadOnlyAll_v2(const TArray<FString>& users, TFunction<void(TSharedPtr<FJsonObject>)> onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUsersGetDataROAll";
		const FString params = FString::Printf(TEXT(R"({"users":%s})"), *HelperArrayToJSONString(users));
		WsRpc.RequestCustom(SMethod, params, [=](TSharedPtr<FJsonValue> result) {
			
			if (onSuccess)
				onSuccess(result->AsObject());

		}, onError);
	}
	void SetUserData(const FString& user, const FString& key, const FString& value, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUserSetData";
		const FString params = FString::Printf(TEXT(R"({"user":"%s","key":"%s","value":%s})"), *user, *key, *value);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}
	void SetUserDataReadOnly(const FString& user, const FString& key, const FString& value, EmptyCB onSuccess, ErrorCB onError) override
	{
		static FString SMethod = "GSUserSetDataRO";
		const FString params = FString::Printf(TEXT(R"({"user":"%s","key":"%s","value":%s})"), *user, *key, *value);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}

#pragma endregion UserData






	void Close(int code = 1000, const FString& reason = FString()) override {
		if (WsRpc.Connection)
		{
			WsRpc.Connection->Close(code, reason);
			WsRpc.Connection = nullptr;
			WsRpc.IdCounter = 0;
		}
	}
	FSourenaGSImpl()
	{

	}
	void Init()
	{
		//bin the notifications
		WsRpc.NotificationCallbacks.Add(TEXT("MatchArrived"), FWsJsonRpc::FNotifyCallbackBase::CreateSP(this, &FSourenaGSImpl::OnNotify_MatchArrived));

	}
	void OnNotify_MatchArrived(TSharedPtr<FJsonValue> params)
	{
		HELPER_PARSE_NOTIFICATION_STRUCT(FGSMatchInfo, OnMatchArrived);
	}
	~FSourenaGSImpl()
	{
		UE_LOG(SourenaBK, Log, TEXT("~FSourenaGSImpl()"));
		Close();
	}


	bool IsTickableWhenPaused() const override { return true; }
	bool IsTickableInEditor() const override { return true; }
	TStatId GetStatId() const override { return TStatId(); }

	void Tick(float DeltaTime) override
	{
		WsRpc.Tick(DeltaTime);
	}
};


TSharedPtr<FSourenaGC> FSourenaGC::Create()
{
	auto ptr = MakeShared<FSourenaGCImpl>(); 
	ptr->Init();
	return ptr;
}
TSharedPtr<FSourenaGS> FSourenaGS::Create()
{
	auto ptr = MakeShared<FSourenaGSImpl>(); 
	ptr->Init();
	return ptr;
}





const FGSMatchUserInfo* FGSMatchInfo::FindUserByTicket(const FString& ticket) const
{
	for (int iTeam = 0; iTeam < Teams.Num(); iTeam++)
	{
		for (int iUser = 0; iUser < Teams[iTeam].Users.Num(); iUser++)
		{
			if (ticket == Teams[iTeam].Users[iUser].Ticket)
				return Teams[iTeam].Users.GetData() + iUser;
		}
	}
	return nullptr;
}



const FGSMatchUserInfo* FGSMatchInfo::FindUserByTicket(const FString& ticket, int& teamIndex, int& userIndex) const
{
	for (int iTeam = 0; iTeam < Teams.Num(); iTeam++)
	{
		for (int iUser = 0; iUser < Teams[iTeam].Users.Num(); iUser++)
		{
			if (ticket == Teams[iTeam].Users[iUser].Ticket) {
				teamIndex = iTeam;
				userIndex = iUser;
				return Teams[iTeam].Users.GetData() + iUser;
			}
				
		}
	}
	return nullptr;
}

TArray<FString> FGSMatchInfo::GetAllUsersUIH() const
{
	TArray<FString> result;
	result.Reserve(Teams.Num() * 5);

	for (int iTeam = 0; iTeam < Teams.Num(); iTeam++)
	{
		for (int iUser = 0; iUser < Teams[iTeam].Users.Num(); iUser++)
		{
			result.Add(Teams[iTeam].Users[iUser].UIH);
		}
	}

	return result;
}

int32 FGSMatchInfo::GetNumUsers() const
{
	int total = 0;
	for (int iTeam = 0; iTeam < Teams.Num(); iTeam++)
		total += Teams[iTeam].Users.Num();
	
	return total;
}

#if WITH_EDITOR
//////////////////////////////////////////////////////////////////////////
class FSourenaACImpl : public FSourenaAC, public FTickableGameObject
{
public:
	FWsJsonRpc WsRpc;

	virtual bool IsAdminClient() const override { return true; }

	void Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError) override
	{
		WsRpc.Connect(url, onSucess, onError);
	}
	void Close(int code = 1000, const FString& reason = FString()) override {
		if (WsRpc.Connection)
		{
			WsRpc.Connection->Close(code, reason);
			WsRpc.Connection = nullptr;
			WsRpc.IdCounter = 0;
		}
	}
	FSourenaACImpl()
	{

	}

	void Init()
	{

	}
	~FSourenaACImpl()
	{
		UE_LOG(SourenaBK, Log, TEXT("~FSourenaACImpl()"));
		Close();
	}

	void Login(const FString& username, const FString& password, EmptyCB onSuccess, ErrorCB onError) 
	{
		static FString SMethod = TEXT("ACLogin");
		FString parms = FString::Printf(TEXT(R"({"user":"%s","pass":"%s"})"), *username, *password);
		WsRpc.RequestErrOnly(SMethod, parms, onSuccess, onError);
	}

	void DataTableAddProperty(const FString& tableName, const FString& propertyName, const FString& propertyValue, EmptyCB onSuccess, ErrorCB onError) 
	{
		static FString SMethod = TEXT("ACDataTableAddProperty");
		FString params = FString::Printf(TEXT(R"({"tableName":"%s","propertyName":"%s","propertyValue":%s})"), *tableName, *propertyName, *propertyValue);
		WsRpc.RequestErrOnly(SMethod, params, onSuccess, onError);
	}

	bool IsTickableWhenPaused() const override { return true; }
	bool IsTickableInEditor() const override { return true; }
	TStatId GetStatId() const override { return TStatId(); }

	void Tick(float DeltaTime) override
	{
		WsRpc.Tick(DeltaTime);
	}
};

TSharedPtr<FSourenaAC> FSourenaAC::Create()
{
	auto ptr = MakeShared<FSourenaACImpl>();
	ptr->Init();
	return ptr;
}

#endif

bool FSourenaInvUserItem::IsInAuction() const
{
	return Auction != FName();
}

float CalcDiscount(float value, float discountAlpha) { return value - (value * discountAlpha); }

int FSourenaInvShopInfo::GetItemFinalPrice(FName currency, FName itemName) const
{
	const FSourenaShopItemInfo* pItem = Items.Find(itemName);
	if (!pItem)
		return -1;
	
	auto discount = FMath::Clamp(Discount + pItem->Discount, 0.0f, 1.0f);
	
	const int32* pPrice = pItem->Prices.Find(currency);
	if (!pPrice)
		return -1;

	float result = CalcDiscount(CalcDiscount(*pPrice, Discount), pItem->Discount);
	return FMath::RoundToInt(result);
}

FSourenaInvUserItem* FSourenaUserInventory::FindItemByInstanceId(const FString& instanceId)
{
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i].InstanceId == instanceId)
			return &Items[i];
	}
	return nullptr;
}
