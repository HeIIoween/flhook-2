#include "FLHook.h"

// Use the class to create and send packets of inconstant size.
#pragma pack(push, 1)
class FLPACKET
{
private:
	uint Size;

	byte kind;
	byte type;

public:
	// This is content of your packet. You may want to reinterpretate it as pointer to packet data struct for convenience.
	byte content[1];

	enum COMMON
	{
		FLPACKET_COMMON_00,
		FLPACKET_COMMON_UPDATEOBJECT,
		FLPACKET_COMMON_FIREWEAPON,
		FLPACKET_COMMON_03,
		FLPACKET_COMMON_SETTARGET,
		FLPACKET_COMMON_CHATMSG,
		FLPACKET_COMMON_06,
		FLPACKET_COMMON_07,
		FLPACKET_COMMON_ACTIVATEEQUIP,
		FLPACKET_COMMON_09,
		FLPACKET_COMMON_0A,
		FLPACKET_COMMON_0B,
		FLPACKET_COMMON_0C,
		FLPACKET_COMMON_0D,
		FLPACKET_COMMON_ACTIVATECRUISE,
		FLPACKET_COMMON_GOTRADELANE,
		FLPACKET_COMMON_STOPTRADELANE,
		FLPACKET_COMMON_SET_WEAPON_GROUP,
		FLPACKET_COMMON_PLAYER_TRADE,
		FLPACKET_COMMON_SET_VISITED_STATE,
		FLPACKET_COMMON_JETTISONCARGO,
		FLPACKET_COMMON_ACTIVATETHRUSTERS,
		FLPACKET_COMMON_REQUEST_BEST_PATH,
		FLPACKET_COMMON_REQUEST_GROUP_POSITIONS,
		FLPACKET_COMMON_REQUEST_PLAYER_STATS,
		FLPACKET_COMMON_SET_MISSION_LOG,
		FLPACKET_COMMON_REQUEST_RANK_LEVEL,
		FLPACKET_COMMON_POP_UP_DIALOG,
		FLPACKET_COMMON_SET_INTERFACE_STATE,
		FLPACKET_COMMON_TRACTOROBJECTS
	};

	enum SERVER
	{
		FLPACKET_SERVER_00,
		FLPACKET_SERVER_CONNECTRESPONSE,
		FLPACKET_SERVER_LOGINRESPONSE,
		FLPACKET_SERVER_CHARACTERINFO,
		FLPACKET_SERVER_CREATESHIP,
		FLPACKET_SERVER_DAMAGEOBJECT,
		FLPACKET_SERVER_DESTROYOBJECT,
		FLPACKET_SERVER_LAUNCH,
		FLPACKET_SERVER_CHARSELECTVERIFIED,
		FLPACKET_SERVER_09,
		FLPACKET_SERVER_ACTIVATEOBJECT,
		FLPACKET_SERVER_LAND,
		FLPACKET_SERVER_0C,
		FLPACKET_SERVER_SETSTARTROOM,
		FLPACKET_SERVER_GFCOMPLETENEWSBROADCASTLIST,
		FLPACKET_SERVER_GFCOMPLETECHARLIST,
		FLPACKET_SERVER_GFCOMPLETEMISSIONCOMPUTERLIST,
		FLPACKET_SERVER_GFCOMPLETESCRIPTBEHAVIORLIST,
		FLPACKET_SERVER_12,
		FLPACKET_SERVER_GFCOMPLETEAMBIENTSCRIPTLIST,
		FLPACKET_SERVER_GFDESTROYNEWSBROADCAST,
		FLPACKET_SERVER_GFDESTROYCHARACTER,
		FLPACKET_SERVER_GFDESTROYMISSIONCOMPUTER,
		FLPACKET_SERVER_GFDESTROYSCRIPTBEHAVIOR,
		FLPACKET_SERVER_18,
		FLPACKET_SERVER_GFDESTROYAMBIENTSCRIPT,
		FLPACKET_SERVER_GFSCRIPTBEHAVIOR,
		FLPACKET_SERVER_1B,
		FLPACKET_SERVER_1C,
		FLPACKET_SERVER_GFUPDATEMISSIONCOMPUTER,
		FLPACKET_SERVER_GFUPDATENEWSBROADCAST,
		FLPACKET_SERVER_GFUPDATEAMBIENTSCRIPT,
		FLPACKET_SERVER_GFMISSIONVENDORACCEPTANCE,
		FLPACKET_SERVER_SYSTEM_SWITCH_OUT,
		FLPACKET_SERVER_SYSTEM_SWITCH_IN,
		FLPACKET_SERVER_SETSHIPARCH,
		FLPACKET_SERVER_SETEQUIPMENT,
		FLPACKET_SERVER_SETCARGO,
		FLPACKET_SERVER_GFUPDATECHAR,
		FLPACKET_SERVER_REQUESTCREATESHIPRESP,
		FLPACKET_SERVER_CREATELOOT,
		FLPACKET_SERVER_SETREPUTATION,
		FLPACKET_SERVER_ADJUSTATTITUDE,
		FLPACKET_SERVER_SETGROUPFEELINGS,
		FLPACKET_SERVER_CREATEMINE,
		FLPACKET_SERVER_CREATECOUNTER,
		FLPACKET_SERVER_SETADDITEM,
		FLPACKET_SERVER_SETREMOVEITEM,
		FLPACKET_SERVER_SETCASH,
		FLPACKET_SERVER_EXPLODEASTEROIDMINE,
		FLPACKET_SERVER_REQUESTSPACESCRIPT,
		FLPACKET_SERVER_SETMISSIONOBJECTIVESTATE,
		FLPACKET_SERVER_REPLACEMISSIONOBJECTIVE,
		FLPACKET_SERVER_SETMISSIONOBJECTIVES,
		FLPACKET_SERVER_36,
		FLPACKET_SERVER_CREATEGUIDED,
		FLPACKET_SERVER_ITEMTRACTORED,
		FLPACKET_SERVER_SCANNOTIFY,
		FLPACKET_SERVER_3A,
		FLPACKET_SERVER_3B,
		FLPACKET_SERVER_REPAIROBJECT,
		FLPACKET_SERVER_REMOTEOBJECTCARGOUPDATE,
		FLPACKET_SERVER_SETNUMKILLS,
		FLPACKET_SERVER_SETMISSIONSUCCESSES,
		FLPACKET_SERVER_SETMISSIONFAILURES,
		FLPACKET_SERVER_BURNFUSE,
		FLPACKET_SERVER_CREATESOLAR,
		FLPACKET_SERVER_SET_STORY_CUE,
		FLPACKET_SERVER_REQUEST_RETURNED,
		FLPACKET_SERVER_SET_MISSION_MESSAGE,
		FLPACKET_SERVER_MARKOBJ,
		FLPACKET_SERVER_CFGINTERFACENOTIFICATION,
		FLPACKET_SERVER_SETCOLLISIONGROUPS,
		FLPACKET_SERVER_SETHULLSTATUS,
		FLPACKET_SERVER_SETGUIDEDTARGET,
		FLPACKET_SERVER_SET_CAMERA,
		FLPACKET_SERVER_REVERT_CAMERA,
		FLPACKET_SERVER_LOADHINT,
		FLPACKET_SERVER_SETDIRECTIVE,
		FLPACKET_SERVER_SENDCOMM,
		FLPACKET_SERVER_50,
		FLPACKET_SERVER_USE_ITEM,
		FLPACKET_SERVER_PLAYERLIST,
		FLPACKET_SERVER_FORMATION_UPDATE,
		FLPACKET_SERVER_MISCOBJUPDATE,
		FLPACKET_SERVER_OBJECTCARGOUPDATE,
		FLPACKET_SERVER_SENDNNMESSAGE,
		FLPACKET_SERVER_SET_MUSIC,
		FLPACKET_SERVER_CANCEL_MUSIC,
		FLPACKET_SERVER_PLAY_SOUND_EFFECT,
		FLPACKET_SERVER_GFMISSIONVENDORWHYEMPTY,
		FLPACKET_SERVER_MISSIONSAVEA
	};

	enum CLIENT
	{
		FLPACKET_CLIENT_00,
		FLPACKET_CLIENT_LOGIN,
		FLPACKET_CLIENT_02,
		FLPACKET_CLIENT_MUNCOLLISION,
		FLPACKET_CLIENT_REQUESTLAUNCH,
		FLPACKET_CLIENT_REQUESTCHARINFO,
		FLPACKET_CLIENT_SELECTCHARACTER,
		FLPACKET_CLIENT_ENTERBASE,
		FLPACKET_CLIENT_REQUESTBASEINFO,
		FLPACKET_CLIENT_REQUESTLOCATIONINFO,
		FLPACKET_CLIENT_GFREQUESTSHIPINFO,
		FLPACKET_CLIENT_SYSTEM_SWITCH_OUT_COMPLETE,
		FLPACKET_CLIENT_OBJCOLLISION,
		FLPACKET_CLIENT_EXITBASE,
		FLPACKET_CLIENT_ENTERLOCATION,
		FLPACKET_CLIENT_EXITLOCATION,
		FLPACKET_CLIENT_REQUESTCREATESHIP,
		FLPACKET_CLIENT_GFGOODSELL,
		FLPACKET_CLIENT_GFGOODBUY,
		FLPACKET_CLIENT_GFSELECTOBJECT,
		FLPACKET_CLIENT_MISSIONRESPONSE,
		FLPACKET_CLIENT_REQSHIPARCH,
		FLPACKET_CLIENT_REQEQUIPMENT,
		FLPACKET_CLIENT_REQCARGO,
		FLPACKET_CLIENT_REQADDITEM,
		FLPACKET_CLIENT_REQREMOVEITEM,
		FLPACKET_CLIENT_REQMODIFYITEM,
		FLPACKET_CLIENT_REQSETCASH,
		FLPACKET_CLIENT_REQCHANGECASH,
		FLPACKET_CLIENT_1D,
		FLPACKET_CLIENT_SAVEGAME,
		FLPACKET_CLIENT_1F,
		FLPACKET_CLIENT_MINEASTEROID,
		FLPACKET_CLIENT_21,
		FLPACKET_CLIENT_DBGCREATESHIP,
		FLPACKET_CLIENT_DBGLOADSYSTEM,
		FLPACKET_CLIENT_DOCK,
		FLPACKET_CLIENT_DBGDESTROYOBJECT,
		FLPACKET_CLIENT_26,
		FLPACKET_CLIENT_TRADERESPONSE,
		FLPACKET_CLIENT_28,
		FLPACKET_CLIENT_29,
		FLPACKET_CLIENT_2A,
		FLPACKET_CLIENT_CARGOSCAN,
		FLPACKET_CLIENT_2C,
		FLPACKET_CLIENT_DBGCONSOLE,
		FLPACKET_CLIENT_DBGFREESYSTEM,
		FLPACKET_CLIENT_SETMANEUVER,
		FLPACKET_CLIENT_DBGRELOCATE_SHIP,
		FLPACKET_CLIENT_REQUEST_EVENT,
		FLPACKET_CLIENT_REQUEST_CANCEL,
		FLPACKET_CLIENT_33,
		FLPACKET_CLIENT_34,
		FLPACKET_CLIENT_INTERFACEITEMUSED,
		FLPACKET_CLIENT_REQCOLLISIONGROUPS,
		FLPACKET_CLIENT_COMMCOMPLETE,
		FLPACKET_CLIENT_REQUESTNEWCHARINFO,
		FLPACKET_CLIENT_CREATENEWCHAR,
		FLPACKET_CLIENT_DESTROYCHAR,
		FLPACKET_CLIENT_REQHULLSTATUS,
		FLPACKET_CLIENT_GFGOODVAPORIZED,
		FLPACKET_CLIENT_BADLANDSOBJCOLLISION,
		FLPACKET_CLIENT_LAUNCHCOMPLETE,
		FLPACKET_CLIENT_HAIL,
		FLPACKET_CLIENT_REQUEST_USE_ITEM,
		FLPACKET_CLIENT_ABORT_MISSION,
		FLPACKET_CLIENT_SKIP_AUTOSAVE,
		FLPACKET_CLIENT_JUMPINCOMPLETE,
		FLPACKET_CLIENT_REQINVINCIBILITY,
		FLPACKET_CLIENT_MISSIONSAVEB,
		FLPACKET_CLIENT_REQDIFFICULTYSCALE,
		FLPACKET_CLIENT_RTCDONE
	};

	// Common packets are being sent from server to client and from client to server.
	static FLPACKET* Create(uint size, COMMON kind)
	{
		FLPACKET* packet = (FLPACKET*)malloc(size + 6);
		packet->Size = size + 2;
		packet->type = 1;
		packet->kind = kind;

		return packet;
	}

	// Server packets are being sent only from server to client.
	static FLPACKET* Create(uint size, SERVER kind)
	{
		FLPACKET* packet = (FLPACKET*)malloc(size + 6);
		packet->Size = size + 2;
		packet->type = 2;
		packet->kind = kind;

		return packet;
	}

	// Client packets are being sent only from client to server. Can't imagine why you ever need to create such a packet at side of server.
	static FLPACKET* Create(uint size, CLIENT kind)
	{
		FLPACKET* packet = (FLPACKET*)malloc(size + 6);
		packet->Size = size + 2;
		packet->type = 3;
		packet->kind = kind;

		return packet;
	}

	// Returns true if sent succesfully, false if not. Frees memory allocated for packet.
	bool SendTo(uint iClientID)
	{
		CDPClientProxy *cdpClient = g_cClientProxyArray[iClientID - 1];

		// We don't include first 4 bytes directly in packet, it is info about size. Type and kind are already included.
		bool result = cdpClient->Send((byte*)this + 4, Size);

		// No mistakes, free allocated memory.
		free(this);

		return result;
	}
};
#pragma pack(pop)
