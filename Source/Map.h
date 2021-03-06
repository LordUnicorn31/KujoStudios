#pragma once

#include <EASTL/list.h>
#include "p2Point.h"
#include "Module.h"

//Testing Map problems of including header

struct Properties
{
	struct Property
	{
		eastl::string name;
		int value;
	};

	~Properties()
	{
		eastl::list<Property*>::iterator item = list.begin();

		while(item != list.end())
		{
			RELEASE(*item);
			item++;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	eastl::list<Property*> list;
};

// ----------------------------------------------------
struct MapLayer
{
	eastl::string	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL),width(0),height(0)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	eastl::string			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tileWidth;
	int					tileHeight;
	SDL_Texture*		texture;
	int					texWidth;
	int					texHeight;
	int					numTilesWidth;
	int					numTilesHeight;
	int					offsetX;
	int					offsetY;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tileWidth;
	int					tileHeight;
	SDL_Color			backgroundColor;
	MapTypes			type;
	eastl::list<TileSet*> tilesets;
	eastl::list<MapLayer*> layers;
};

// ----------------------------------------------------
class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	//Testing and Debug purposes
	void DrawGrid();

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	void GetMapSize(int& w, int& h) const;

	TileSet* GetTilesetFromTileId(int id) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);


public:

	MapData data;

private:

	pugi::xml_document	mapFile;
	eastl::string			folder;
	bool				mapLoaded;

};