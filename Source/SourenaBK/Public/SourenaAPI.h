#pragma once

#include "CoreMinimal.h"
#include "Json.h"

#include "SourenaAPI.generated.h"

//////////////////////////////////////////////////////////////////////////
class FSourenaBaseAPI;
class FSourenaGC;
class FSourenaGS;

USTRUCT(BlueprintType)
struct SOURENABK_API FGameModeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MinUser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxUser;
	
};

USTRUCT(BlueprintType)
struct SOURENABK_API FRpcError
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Code;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;
};

/*
result of Game Client login.
*/
USTRUCT(BlueprintType)
struct SOURENABK_API FGCLoginResult
{
	GENERATED_BODY()
	
	//unique id of the logged in user
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserId;
	//true means the account created during the login. (was not exists before)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool NewlyCreated;
	//the session token that could be used for future logins. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SessionToken;
	
};



USTRUCT(BlueprintType)
struct SOURENABK_API FGCMatchmakeParams
{
	GENERATED_BODY()
	//name of the game mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GameMode;
	//name of the build
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Build;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> UserTags;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGCMatchmakeResult
{
	GENERATED_BODY()

	//a unique ticket that lets the player to join the GameServer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerTicket;
	//address of the game server
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Address;
	//port that game server is listening on
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Port;
	//lobby id of the game server
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LobbyId;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FUserBanInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBanned;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Reason;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BanBy;
	//time at which user got banned
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BanTime;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ExpireTime;
	//user ban will be removed after @ExpiresAfter seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ExpiresAfter;

};

USTRUCT(BlueprintType)
struct SOURENABK_API FGSRegisterParams
{
	GENERATED_BODY()

	//lobbyToken that GameServer received by command line
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LobbyToken;
	//IP address of the game server. if empty, the address of supervisor will be used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Address;
	//port that game server is listening on.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Port;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGSRegisterResult
{
	GENERATED_BODY()

	//unique id of the registered lobby
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LobbyId;
	//name of the build
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BuildName;
	//information of the game mode the GameServer launched with
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameModeInfo GameMode;
	//include tags that game server launched with
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Includes;
	//the maximum time we wait for the match. in seconds
	UPROPERTY()
	float RestTime;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGSPlayerJoinResult
{
	GENERATED_BODY()

	//unique id of the user
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LoginBy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> MatchmakeUserTags;
};

UENUM(BlueprintType)
enum class ESourenaFriendRelationship : uint8
{
	Pending,
	Accepted,
	Declined,
	Blocked,

};

USTRUCT()
struct SOURENABK_API FGCGetRelationshipResult
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FString> Hashes;
	UPROPERTY()
	TArray<FString> DisplayNames;
	UPROPERTY()
	TArray<int> Relations;
};

//information about relationship to a user
USTRUCT(BlueprintType)
struct SOURENABK_API FGCRelationshipInfo
{
	GENERATED_BODY()
	//user we have relation to
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UIH;
	//user who did the last action (blocking, accepting, ...) its either us or other
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ActionUIH;
	//display name of the user we have relation to
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	//current status of the relation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESourenaFriendRelationship Relationship;
};

UENUM(BlueprintType)
enum class ERelationUpdateAction : uint8
{
	//other accepted my friendship
	Accepted,
	//other declined my friendship
	Declined,
	//other requested friendship with me
	Requested,
	//other blocked me
	Blocked,
	//other unblocked me
	Unblocked,
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGCRelationUpdateNotification
{
	GENERATED_BODY()

	//the user who did the action
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OtherUIH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	//the action he did
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERelationUpdateAction Action;
};



USTRUCT()
struct SOURENABK_API FGCFindUserLikeResult
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FString> Hashes;
	UPROPERTY()
	TArray<FString> Emails;
	UPROPERTY()
	TArray<FString> Usernames;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGCUserFindInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UIH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Username;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Email;
};


USTRUCT(BlueprintType)
struct SOURENABK_API FGCPrivateMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime Time;
	//UUID of the message
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MessageId;
	//UserIdHash of the sender
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sender;
	//valid if this was received by notification
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SenderDisplayName;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartyUserInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Ready;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> Data;
};
	

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartyInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FSourenaPartyUserInfo> joinedUsers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> PublicData;
	
};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartyMessage
{
	GENERATED_BODY()

	//the user who sent the message
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sender;
	//display name of the sender, might be username or nickname
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SenderDisplayName;
	//the message 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;
	//id of the party this message belongs to
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;

};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartyInviteNotification
{
	GENERATED_BODY()

	//the user who sent us the invitation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sender;
	//display name of the sender, might be username or nickname
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SenderDisplayName;
	//the ticket which lets us to join to the party. this ticket is only valid for guest
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Ticket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartyUserReadyNotification
 {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;
	//the user whose state changed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Ready;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool MatchStarted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool MatchCanceled;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGCMatchReadyNotification
{
	GENERATED_BODY()

	//a unique ticket that lets the player to join the GameServer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Ticket;
	//address of the game server as IP:PORT
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Address;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MatchId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LobbyId;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGCMatchStatusNotification
{
	GENERATED_BODY()

	//number of players in the queue
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumUsers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MatchId;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGCMatchFailNotification
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Reason;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MatchId;
};




USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartyDataNotification
{
	GENERATED_BODY()

	//the user whose data changed, or the user who changed the data. based on the notification we receive (public or private)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartyUserJoinLeaveNotification
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;
	//the user who joined or left
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString User;
	//display name of the user
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaPartySetReadyResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;
	//true if matchmake started
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool MatchStarted;
	//valid if 'MatchStarted' is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MatchTicket;
};

UENUM(BlueprintType)
enum class EAccountProvider : uint8
{
	Invalid = 0,
	CustomId,
	EmailPass,
	Steam,
};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaUserPresence
{
	GENERATED_BODY()

	//the way that user is logged in. zero if user not found or offline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAccountProvider LoginBy;
	//invalid if user is offline or not found
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	//id of the party that user is joined, if not empty. invalid if user is offline or not found
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PartyId;

};

USTRUCT(BlueprintType)
struct SOURENABK_API FSourenaFriendOnOffNotification
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FriendUIH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
};


USTRUCT(BlueprintType)
struct SOURENABK_API FGSMatchUserInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UIH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, NotReplicated)
	FString Ticket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGSMatchTeamInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGSMatchUserInfo> Users;
};

USTRUCT(BlueprintType)
struct SOURENABK_API FGSMatchInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MathId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MatchQueue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGSMatchTeamInfo> Teams;

	const FGSMatchUserInfo* FindUserByTicket(const FString& ticket) const;
	const FGSMatchUserInfo* FindUserByTicket(const FString& ticket, int& teamIndex, int& userIndex) const;
	//returns the UIH of all the players in this match
	TArray<FString> GetAllUsersUIH() const;
	//return the total number of users
	int32 GetNumUsers() const;
};



#pragma region Inventory Structs

struct FSourenaItemCash
{
	FName Currency;
	int Value;
};
struct FSourenaItemBundle
{
	TArray<FName> Items;
};
struct SOURENABK_API FSourenaInvItemInfo
{
	FName Name;
	FName Type;
	FSourenaItemCash CashInfo;
	FSourenaItemBundle BundleInfo;
	TSharedPtr<FJsonValue> CustomData;
	int MaxAge;
};

struct SOURENABK_API FSourenaShopItemInfo
{
	//secondary discount for this item
	float Discount;
	//defined prices for each virtual currency. its not the final price. discount must be calculated
	TMap<FName, int> Prices;
};

struct SOURENABK_API FSourenaInvShopInfo
{
	//name of the shop
	FName Name;
	//
	TSharedPtr<FJsonValue> CustomData;
	//primary discount applied to all items
	float Discount;
	//all the purchasable items
	TMap<FName, FSourenaShopItemInfo> Items;

	int GetItemFinalPrice(FName currency, FName itemName) const;
};

struct SOURENABK_API FSourenaInvAuctionInfo
{
	FName Name;
	FName Currency;
	int MinPrice;
	int MaxPrice;
	int DefaultPrice;
	int BuyerTax;
	int MaxTime;
	TSharedPtr<FJsonValue> CustomData;
};

struct SOURENABK_API FSourenaInvCurrencyInfo
{
	FName Name;
	int DefaultValue;
	FName ImageURL;
};
struct SOURENABK_API FSourenaInvAggregateInfo
{
	FName BaseItem;
	TMap<FName, TArray<FName>> Slots;
	TSharedPtr<FJsonValue> CustomData;
};
struct SOURENABK_API FSourenaInvDefinitions
{
	TMap<FName, FSourenaInvItemInfo> Items;
	TMap<FName, FSourenaInvShopInfo> Shops;
	TMap<FName, FSourenaInvCurrencyInfo> Currencies;
	TMap<FName, FSourenaInvAggregateInfo> Aggregates;
	TMap<FName, FSourenaInvAuctionInfo> Auctions;

	
};

struct SOURENABK_API FSourenaInvUserItem
{
	FString InstanceId;
	FString ItemName;

	FName ReceiveBy;
	FDateTime ReceiveTime;
	FDateTime ExpireTime;

	FName Auction;
	FDateTime AuctionStartTime;
	FDateTime AuctionEndTime;
	int AuctionPrice;
	int AuctionLastPrice;
	FDateTime AuctionLastBidTime;

	bool IsInAuction() const;
};

struct SOURENABK_API FSourenaUserInventory
{
	//virtual currencies of the user
	TMap<FName, int> Currencies;
	//all the items related to this user even those who are in the auctions
	TArray<FSourenaInvUserItem> Items;

	FSourenaInvUserItem* FindItemByInstanceId(const FString& instanceId);
	
};

struct FSourenaAggregateCreationParams
{
	FName BaseItemName;
	FString BaseIntanceId;
	TMap<FName, FString> Attachments;
};


USTRUCT(BlueprintType)
struct FSourenaUserInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Uih;
};


#pragma endregion


/*
*/
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PrivateMessage, const FGCPrivateMessage&);

//#DEPRECATED packed to one unit
//DECLARE_MULTICAST_DELEGATE_TwoParams(FNotify_FriendshipAccepted, TSharedRef<FSourenaGC>, FString&);
//DECLARE_MULTICAST_DELEGATE_TwoParams(FNotify_FriendshipDeclined, TSharedRef<FSourenaGC>, FString&);
//DECLARE_MULTICAST_DELEGATE_TwoParams(FNotify_ReceivedFriendRequest, TSharedRef<FSourenaGC>, FString&);
//DECLARE_MULTICAST_DELEGATE_TwoParams(FNotify_RelationBlocked, TSharedRef<FSourenaGC>, FString&);
//DECLARE_MULTICAST_DELEGATE_TwoParams(FNotify_RelationUnblocked, TSharedRef<FSourenaGC>, FString&);

DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_RelationUpdate, const FGCRelationUpdateNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_FriendOnline, const FSourenaFriendOnOffNotification&);

DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_DebugMessage, const FString&);


DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PartyMessage, const FSourenaPartyMessage&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PartyInvite, const FSourenaPartyInviteNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PartyUserLeft, const FSourenaPartyUserJoinLeaveNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PartyUserJoined, const FSourenaPartyUserJoinLeaveNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PartyUserReady, const FSourenaPartyUserReadyNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PartyPublicDataUpdate, const FSourenaPartyDataNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_PartyPrivateDataUpdate, const FSourenaPartyDataNotification&);

DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_MatchReady, const FGCMatchReadyNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_MatchStatus, const FGCMatchStatusNotification&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_MatchFailed, const FGCMatchFailNotification&);

DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_GSMatchArrived, const FGSMatchInfo&);


/*
{
	""	
}
*/
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_InvBidLost, const FGSMatchInfo&);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotify_InvBidWon, const FGSMatchInfo&);
 
USTRUCT(BlueprintType)
struct FSourenaDataTable
{
	GENERATED_BODY()

	//name of the table
	FName TableName;
	//key is name of the property. value can be any type of json value
	TMap<FName, TSharedPtr<FJsonValue>> Properties;
};

/*

*/
class FSourenaBaseAPI : public TSharedFromThis<FSourenaBaseAPI>
{
public:
	typedef TFunction<void(FRpcError&)> ErrorCB;
	typedef TFunction<void()> EmptyCB;
	typedef TFunction<void(FString&)> StringCB;
	typedef TFunction<void(TMap<FString, FString>&)> StringMapCB;
	typedef TFunction<void(TMap<FString, TMap<FName, FString>>&)> StringNameStringMapCB;

	typedef TFunction<void(TMap<FString, TSharedPtr<FJsonValue>>)> StringJsonCB;

	virtual bool IsGameClient() const { return false; }
	virtual bool IsGameServer() const { return false; }
	virtual bool IsAdminClient() const { return false; }

	/*
	fetch the read only data table. (the result could be used used for adjusting properties dynamically, for instance match length, kill score, etc)
	both GameClient and GameServer are allowed to call this function after a successful login or registration
	*/
	virtual void GetDataTable(const FString& tableName, TFunction<void(FSourenaDataTable&)> onSuccess, ErrorCB onError) {}


	/*
	return all the defined stuff related to inventory (items, shops, ...)
	*/
	virtual void InvGetDefinitions(TFunction<void(FSourenaInvDefinitions&)> onSuccess, ErrorCB onError) {}

	virtual ~FSourenaBaseAPI() {}
};


/*
GameClient APIs.
*/
class SOURENABK_API FSourenaGC : public FSourenaBaseAPI
{
public:
	FGCLoginResult LoginResult;
	//creates and returns the implementation. keep the result somewhere. returned connection will get closed if get destroyed.
	static TSharedPtr<FSourenaGC> Create();

	//connection will be closed if this object dies.
	virtual ~FSourenaGC() {}

	//establish a WebSocket connection to the specified URL. supports both ws:// and wss://
	//after a successful connection you should call one of the Login functions.
	virtual void Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError) {}
	//close the websocket connection
	virtual void Close(int code = 1000, const FString& reason = FString()) {}

	/*
	@param	identiy		username or email address to login with.
	@param	password	password to login with.
	*/
	virtual void LoginEmailPass(const FString& identiy, const FString& password, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError) {}


	/*
	login by the session token. its used to implement automatic login without re entering username and password.
	@param	sessionToken	the token generated by the previous login.
	*/
	virtual void LoginByToken(const FString& sessionToken, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError) {}
	/*
	login by steam account.
	@param		ticket		the ticket generated by steam api. must be hex encoded.
	@param		bCreate		if true and no account with such a steamId found. create a new one.
	*/
	virtual void LoginSteam(const FString& ticket, bool bCreate, TFunction<void(FGCLoginResult&)> onSuccess, ErrorCB onError) {}


	//the following functions all need a logged-in user. you should first login with one of the method available above.

	virtual void Matchmake(const FGCMatchmakeParams& params, TFunction<void(FGCMatchmakeResult&)> onSuccess, ErrorCB onError) {}
	virtual void ReportBan(const FString& reason, int durationMinutes, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void GetBanStatus(TFunction<void(FUserBanInfo&)> onSuccess, ErrorCB onError) {}
	virtual void Logout(EmptyCB onSuccess, ErrorCB onError) {}


	virtual void RequestFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void CancelPendingFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void AcceptFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void DeclineFriendship(const FString& uih, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void RelationBlockUser(const FString& uih, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void RelationUnblockUser(const FString& uih, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void GetRelationships(TFunction<void(TArray<FGCRelationshipInfo>&)> onSuccess, ErrorCB onError) {}

	virtual void FindUserLike(const FString like, TFunction<void(TArray<FGCUserFindInfo>&)> onSuccess, ErrorCB onError) {}

	virtual void FriendSendPrivateChat(const FString& friendUIH, const FString& message, TFunction<void(FGCPrivateMessage&)> onSuccess, ErrorCB onError) {}
	virtual void FriendGetAllPrivateChats(const FString& friendUIH, TFunction<void(TArray<FGCPrivateMessage>&)> onSuccess, ErrorCB onError) {}


	virtual void PartyCreate(const FString& partyClass, TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) {}
	virtual void PartyLeave(EmptyCB onSuccess, ErrorCB onError) {}
	virtual void PartyInvite(const FString& guest, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void PartyJoin(const FString& ticket, TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) {}
	virtual void PartyJoinDirect(const FString& user, TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) {}
	virtual void PartyGetInfo(TFunction<void(FSourenaPartyInfo&)> onSuccess, ErrorCB onError) {}
	virtual void PartySetPublicData(const FString& key, const FString& value, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void PartySetPrivateData(const FString& key, const FString& value, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void PartySetReady(bool bReady, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void PartySendMessage(const FString& message, EmptyCB onSuccess, ErrorCB onError) {}

	virtual void GetUserPresence(const FString& user, TFunction<void(FSourenaUserPresence&)> onSuccess, ErrorCB onError) {}

#pragma region Inventory

	/*
	returns the inventory of current logged in user. contains currencies, item instances, etc
	*/
	virtual void InvGetUserInventory(TFunction<void(FSourenaUserInventory&)> onSuccess, ErrorCB onError) {}
	/*
	donate an item to another user directly. the doner should owns the specified item.
	@itemInstanceId		item to be donated
	@receiver			the user who receives the item
	*/
	virtual void InvDonateItem(const FString& itemInstanceId, const FString& receiver, EmptyCB onSuccess, ErrorCB onError) {}
	/*
	try to buy an item from the shop with virtual currency.
	@shopName		name of the shop to buy from
	@itemName		name of the item to buy
	@currency		name of currency to buy with
	@price			this must be the price that user see.
	*/
	virtual void InvBuyItem(FName shopName, FName itemName, FName currency, int price) {}
	/*
	#DevelopmentOnly
	grants an item to the user.
	*/
	virtual void InvGrantItem(FName itemName, TFunction<void(FSourenaInvUserItem&)> onSuccess, ErrorCB onError) {}
	/*
	remove an item from the user inventory. the item should not be in auction.
	*/
	virtual void InvRemoveItem(const FString& itemInstanceId) {}
	/*
	opens a box and takes the collected items.
	on success it gives an array of collected items. if any.
	*/
	virtual void InvOpenBox(const FString& itemInstanceId, TFunction<void(TArray<FSourenaInvUserItem>&)> onSuccess, ErrorCB onError) {}
	/*
	places the specified item in the auction for selling.
	*/
	virtual void InvAuctionItem(FName autionName, const FString& itemInstanceId, int suggestedPrice) {}
	/*
	bid an item of the auction. 
	*/
	virtual void InvAuctionBid(const FString& instanceId, int price) {}
	/*
	retrieves the items available in the auction
	*/
	virtual void InvGetAuctionItems(FName auction, FDateTime beforeThis, int limit, TFunction<void(TArray<FSourenaInvUserItem>&)> onSuccess, ErrorCB onError) {}
	/*
	make an aggregate item from the specified items
	*/
	virtual void InvAggregateItems(const FSourenaAggregateCreationParams& params, TFunction<void(FSourenaInvUserItem&)> onSuccess, ErrorCB onError) {}

#pragma endregion Inventory


	//UserData
	virtual void GetUserDataReadOnly(const FString& key, StringCB onSuccess, ErrorCB onError) {}
	virtual void GetUserDataReadOnlyAll(StringMapCB onSuccess, ErrorCB onError) {}
	virtual void GetUserData(const FString& key, StringCB onSuccess, ErrorCB onError) {}
	virtual void GetUserDataAll(StringMapCB onSuccess, ErrorCB onError) {}
	virtual void GetUserDataAll_v2(TFunction<void(TSharedPtr<FJsonObject>)> onSuccess, ErrorCB onError) {}
	/*
	@key			the string key
	@jsonValue		any json value.
	*/
	virtual void SetUserData(const FString& key, const FString& jsonValue, EmptyCB onSuccess, ErrorCB onError) {}




	//this is called when a friend send us a private message
	FNotify_PrivateMessage				OnPrivateMessage;
	//this is called when something related to relation happens. (someone accept our friendship, decline, block , ...)
	FNotify_RelationUpdate				OnRelationUpdate;
	//this is called when one of our friends becomes online
	FNotify_FriendOnline				OnFriendOnline;
	//this is called when one of our friends becomes offline
	FNotify_FriendOnline				OnFriendOffline;



	//this is called when a user of the party (including ourself) sends a message.
	FNotify_PartyMessage				OnPartyMessage;
	//this is called when someone (a friend at the moment) sends us a invitation
	FNotify_PartyInvite					OnPartyInvite;
	//this is called when someone joined to our party, excluding ourself
	FNotify_PartyUserJoined				OnPartyUserJoined;
	//this is called when someone left the party. excluding ourself
	FNotify_PartyUserLeft				OnPartyUserLeft;
	/*
	this is received when a user of the party becomes ready or unready. including ourself. 
	parameters of the struct shows whether the match is started, canceled, or nothing happened.
	*/
	FNotify_PartyUserReady				OnPartyUserReady;
	//this is called when a party's user changes a field of the public data
	FNotify_PartyPublicDataUpdate		OnPartyPublicDataUpdate;
	//this is called when a party's user changes a field of his own private data.
	FNotify_PartyPrivateDataUpdate		OnPartyPrivateDataUpdate;


	//this notification is received when match is ready and game client can connect to the game server associated with it.
	FNotify_MatchReady					OnMatchReady;
	//this is received once in while to tell us about the status of the current in-progress match ticket
	FNotify_MatchStatus					OnMatchStatus;
	//this is received when match failed. is different than 'Cancel'
	FNotify_MatchFailed					OnMatchFailed;



	FNotify_DebugMessage				OnDebugMessage;
};


/*
GameServer APIs
*/
class SOURENABK_API FSourenaGS : public FSourenaBaseAPI
{
public:
	FGSRegisterResult RegisterResult;

	//creates and returns the implementation. keep the result somewhere. returned connection will get closed if get destroyed.
	static TSharedPtr<FSourenaGS> Create();

	virtual ~FSourenaGS() {}

	virtual void Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError) {}
	virtual void Close(int code = 1000, const FString& reason = FString()) {}
	/*
	register the game server.
	this should be called once after a successful Connect.
	*/
	virtual void Register(const FGSRegisterParams& params, TFunction<void(FGSRegisterResult&)> onSuccess, ErrorCB onError) {}

	//the following functions require a successful Register.

	//unregister the game server. this must be called before exiting of the process or end of the game. 
	//unregistering the lobby will disjoin the users and they will be allowed to matchmake again.
	virtual void Unregister(EmptyCB onSuccess, ErrorCB onError) {}
	/*
	verify that user joined to the game server.
	this must be called when GameServer receives a new GameClient connection.
	@param	ticket		player ticket that GameClient received from matchmake.
	*/
	virtual void PlayerJoin(const FString& ticket, TFunction<void(FGSPlayerJoinResult&)> onSuccess, ErrorCB onError) {}
	/*
	disjoin the user from the lobby. based on the game type or map you may or may not call this.
	in lobby room call this when user get disconnected. (to open a empty slot for another user)
	@param	ticket		player ticket that GameClient received from matchmake.
	*/
	virtual void PlayerLeave(const FString& ticket, EmptyCB onSuccess, ErrorCB onError) {}
	
	virtual void ReportBan(const FString& ticket, const FString& reason, int durationMinutes, EmptyCB onSuccess, ErrorCB onError) {}
	/*
	lock the lobby so that no more user can join.
	*/
	virtual void LockLobby(EmptyCB onSuccess, ErrorCB onError) {}

	

	//UserData
	virtual void GetUserDataReadOnly(const FString& user, const FString& key, StringCB onSuccess, ErrorCB onError) {}
	virtual void GetUserDataReadOnlyAll(const FString& user, StringMapCB onSuccess, ErrorCB onError) {}
	virtual void GetUsersDataReadOnlyAll(const TArray<FString>& users, StringNameStringMapCB onSuccess, ErrorCB onError) {}

	virtual void GetUsersDataReadOnlyAll_v2(const TArray<FString>& users, TFunction<void(TSharedPtr<FJsonObject>)> onSuccess, ErrorCB onError) {}

	virtual void GetUserData(const FString& user, const FString& key, StringCB onSuccess, ErrorCB onError) {}
	virtual void GetUserDataAll(const FString& user, StringMapCB onSuccess, ErrorCB onError) {}
	virtual void GetUsersDataAll(const TArray<FString>& users, StringNameStringMapCB onSuccess, ErrorCB onError) {}
	/*
	the json result is as follow
	{
		"user0" : {
			"property1" : value,
			"property2" : value2,
		},
		...
	*/
	virtual void GetUsersDataAll_v2(const TArray<FString>& users, TFunction<void(TSharedPtr<FJsonObject>)> onSuccess, ErrorCB onError) {}

	virtual void SetUserData(const FString& user, const FString& key, const FString& jsonValue, EmptyCB onSuccess, ErrorCB onError) {}
	virtual void SetUserDataReadOnly(const FString& user, const FString& key, const FString& jsonValue, EmptyCB onSuccess, ErrorCB onError) {}

#pragma region Inventory

	virtual void InvGetUsersInventory(const TArray<FName>& users, TFunction<void(TArray<FSourenaUserInventory>&)> onSuccess, ErrorCB onError) {}
	virtual void InvGrantItem(FName receiver, FName itemName, TFunction<void(FSourenaInvUserItem&)> onSuccess, ErrorCB onError) {}

#pragma endregion Inventory

	/*
	increases the user's rank
	@params value	the positive value to be added to the current rank
	*/
	virtual void IncUserRank(const FString& user, int value, EmptyCB onSuccess, ErrorCB onError) {}

	FNotify_GSMatchArrived OnMatchArrived;
};

#if WITH_EDITOR
/*
AdminClient APIs
*/
class SOURENABK_API FSourenaAC : public FSourenaBaseAPI
{
public:
	//creates and returns the implementation. keep the result somewhere. returned connection will get closed if get destroyed.
	static TSharedPtr<FSourenaAC> Create();


	virtual void Connect(const FString& url, TFunction<void()> onSucess, TFunction<void(const FString&)> onError) {}
	virtual void Close(int code = 1000, const FString& reason = FString()) {}

	virtual void Login(const FString& username, const FString& password, EmptyCB onSuccess, ErrorCB onError) {}
	
	/*
	@propertyValue		must be a valid json value. e.g "string", [1,2,3], {}, ...
	*/
	virtual void DataTableAddProperty(const FString& tableName, const FString& propertyName, const FString& propertyValue, EmptyCB onSuccess, ErrorCB onError) {}

};
#endif