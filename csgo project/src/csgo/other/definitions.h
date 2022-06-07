#pragma once

#define TICK_INTERVAL			( i::Globals->flIntervalPerTick )
#define TIME_TO_TICKS( t )		( static_cast<int>( 0.5f + static_cast<float>( t ) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL * static_cast<float>( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		( -1 )

enum ESolidType : int
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP,		// a bsp tree
	SOLID_BBOX,		// an aabb
	SOLID_OBB,		// an obb (not implemented yet)
	SOLID_OBB_YAW,	// an obb, constrained so that it can only yaw
	SOLID_CUSTOM,	// always call into the entity for tests
	SOLID_VPHYSICS,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST
};

enum EMoveType : int
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

enum ELifeState : int
{
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

enum EntityFlags
{
	FL_ONGROUND = (1 << 0), 	// At rest / on the ground
	FL_DUCKING = (1 << 1),		// Player flag -- Player is fully crouched
	FL_WATERJUMP = (1 << 2),	// player jumping out of water
	FL_ONTRAIN = (1 << 3),		// Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
	FL_INRAIN = (1 << 4),		// Indicates the entity is standing in rain
	FL_FROZEN = (1 << 5),		// Player is frozen for 3rd person camera
	FL_ATCONTROLS = (1 << 6),	// Player can't move, but keeps key inputs for controlling another entity
	FL_CLIENT = (1 << 7),		// Is a player
	FL_FAKECLIENT = (1 << 8),	// Fake client, simulated server side; don't send network messages to them
	FL_INWATER = (1 << 10),		// In water
};

typedef struct
{
	int		netID;
	int		sequenceNumber;
	int		packetID : 16;
	int		nSplitSize : 16;
} SPLITPACKET;

#define MAX_AREA_STATE_BYTES		32
#define MAX_AREA_PORTAL_STATE_BYTES 24

#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER			"Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"

#define TEXTURE_SKYBOX_BAGGAGE						"cs_baggage_skybox_"
#define TEXTURE_SKYBOX_TIBET						"cs_tibet"
#define TEXTURE_SKYBOX_EMBASSY						"embassy"
#define TEXTURE_SKYBOX_ITALY						"italy"
#define TEXTURE_SKYBOX_JUNGLE						"jungle"
#define TEXTURE_SKYBOX_NUKEBLANK					"nukeblank"
#define TEXTURE_SKYBOX_OFFICE						"office"
#define TEXTURE_SKYBOX_DAYLIGHT01					"sky_cs15_daylight01_hdr"
#define TEXTURE_SKYBOX_DAYLIGHT02					"sky_cs15_daylight02_hdr"
#define TEXTURE_SKYBOX_DAYLIGHT03					"sky_cs15_daylight03_hdr"
#define TEXTURE_SKYBOX_DAYLIGHT04					"sky_cs15_daylight04_hdr"
#define TEXTURE_SKYBOX_CLOUDY01						"sky_csgo_cloudy01"
#define TEXTURE_SKYBOX_NIGHT02						"sky_csgo_night02"
#define TEXTURE_SKYBOX_NIGHT02B						"sky_csgo_night02b"
#define TEXTURE_SKYBOX_DUST							"sky_dust"
#define TEXTURE_SKYBOX_VENICE						"sky_venice"
#define TEXTURE_SKYBOX_VERTIGO						"vertigo"
#define TEXTURE_SKYBOX_VERTIGO_HDR					"vertigoblue_hdr"
#define TEXTURE_SKYBOX_VIETNAM						"vietnam"
#define NET_MAX_MESSAGE 523956
#define MAX_ROUTABLE_PAYLOAD		1260	// Matches x360 size
#define MAX_SPLIT_SIZE	(1260 - sizeof( SPLITPACKET ))
#define MIN_SPLIT_SIZE	(576 - sizeof( SPLITPACKET ))
#define MAX_SPLITPACKET_SPLITS ( NET_MAX_MESSAGE / MIN_SPLIT_SIZE )
#define NET_HEADER_FLAG_SPLITPACKET				-2
#define NET_HEADER_FLAG_COMPRESSEDPACKET		-3
#define CONNECTIONLESS_HEADER			0xFFFFFFFF	// all OOB packet start with this sequence
#define PACKET_FLAG_RELIABLE			(1<<0)	// packet contains subchannel stream data
#define PACKET_FLAG_COMPRESSED			(1<<1)	// packet is compressed
#define PACKET_FLAG_ENCRYPTED			(1<<2)  // packet is encrypted
#define PACKET_FLAG_SPLIT				(1<<3)  // packet is split
#define PACKET_FLAG_CHOKED				(1<<4)  // packet was choked by sender
#define ENCODE_PAD_BITS( x ) ( ( x << 5 ) & 0xff )
#define BITS_PER_INT		32