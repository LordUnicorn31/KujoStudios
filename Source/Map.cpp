#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
	name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = config.child("folder").child_value();

	return ret;
}

void Map::Draw()
{
	if(mapLoaded == false)
		return;

	iPoint cam_size(WorldToMap(-App->render->camera.x + App->render->camera.w, -App->render->camera.y + App->render->camera.h));
	iPoint cam_pos = WorldToMap(-App->render->camera.x + 200, -App->render->camera.y);//el mes 200 es pk hi ha el panel k fa 200 i no volem dibuixar mapa per sota del panel
	eastl::list<MapLayer*>::const_iterator item;
	for(item = data.layers.cbegin(); item != data.layers.end(); item = next(item))
	{
		MapLayer* layer = *item;
		if (layer->properties.Get("Navigation", 0) == 1)
			continue;

		for(int y = cam_pos.y; y <= cam_size.y; ++y)
		{
			if (y<0 || y>=data.height)
				continue;
			for(int x = cam_pos.x; x <= cam_size.x; ++x)
			{
				if (x<0 || x>data.width)
					continue;
				int tile_id = layer->Get(x, y);
				if(tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);

					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = MapToWorld(x, y);

					App->render->Blit(tileset->texture, pos.x, pos.y, &r);
				}
			}
		}
	}
}

int Properties::Get(const char* value, int default_value) const
{
	eastl::list<Property*>::const_iterator item = list.cbegin();

	while(item != list.end())
	{
		if((*item)->name == value)
			return (*item)->value;
		item = next(item);
	}

	return default_value;
}

void Map::DrawGrid()
{
	iPoint point_a = {data.tileWidth /2 +3,data.tileHeight / 2 +3};
	iPoint point_b = { -((data.width * data.tileWidth) / 2 + data.tileWidth /2) + data.tileWidth +3,
		(data.height * data.tileHeight) /2 + data.tileHeight / 2 +3};

	for (int i = 0; i <= data.width; ++i)
	{
		App->render->DrawLine(point_a.x, point_a.y, point_b.x, point_b.y, 255, 255, 255,50);

		point_a.x += data.tileWidth / 2;
		point_a.y += data.tileHeight / 2;

		point_b.x += data.tileWidth / 2;
		point_b.y += data.tileHeight / 2;
	}

	//Back to the first tile
	point_a.x = data.tileWidth / 2 ;
	point_a.y = data.tileHeight / 2 + 3;

	point_b.x = (data.width * data.tileWidth) / 2 + data.tileWidth / 2 ;
	point_b.y = (data.height * data.tileHeight) / 2 + data.tileHeight / 2 +3;

	for (int i = 0; i <= data.height; ++i)
	{
		App->render->DrawLine(point_a.x, point_a.y, point_b.x, point_b.y, 255, 255, 255, 50);

		point_a.x -= data.tileWidth / 2;
		point_a.y += data.tileHeight / 2;

		point_b.x -= data.tileWidth / 2;
		point_b.y += data.tileHeight / 2;
	}
}

TileSet* Map::GetTilesetFromTileId(int id) const
{
	eastl::list<TileSet*>::const_iterator item = data.tilesets.cbegin();
	TileSet* set = *item;

	while(item != data.tilesets.end())
	{
		if(id < (*item)->firstgid)
		{
			set = *prev(item);
			break;
		}
		set = *item;
		item = next(item);
	}

	return set;
}

void Map::GetMapSize(int& w, int& h) const
{
	w = App->map->data.width * App->map->data.tileWidth;
	h = App->map->data.height * App->map->data.tileHeight;
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tileWidth;
		ret.y = y * data.tileHeight;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tileWidth * 0.5f);
		ret.y = (x + y) * (data.tileHeight * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tileWidth * 0.5f;
		float half_height = data.tileHeight * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2) - 1;
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relative_id % numTilesWidth));
	rect.y = margin + ((rect.h + spacing) * (relative_id / numTilesWidth));
	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	eastl::list<TileSet*>::iterator item = data.tilesets.begin();

	while(item != data.tilesets.end())
	{
		App->tex->UnLoad((*item)->texture);
		RELEASE(*item);
		item = next(item);
	}
	data.tilesets.clear();

	// Remove all layers

	eastl::list<MapLayer*>::iterator item2 = data.layers.begin();

	while(item2 != data.layers.end())
	{
		RELEASE(*item2);
		item2 = next(item2);
	}
	data.layers.clear();

	// Clean up the pugui tree
	mapFile.reset();

	return true;
}

// Load new map
bool Map::Load(const char* file_name)
{
	bool ret = true;
	eastl::string tmp;
	tmp = folder.data();
	tmp = tmp + file_name;

	pugi::xml_parse_result result = mapFile.load_file(tmp.data());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.push_back(lay);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tileWidth, data.tileHeight);

		eastl::list<TileSet*>::const_iterator item = data.tilesets.cbegin();
		while(item != data.tilesets.end())
		{
			TileSet* s = *item;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tileWidth, s->tileHeight);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = next(item);
		}

		eastl::list<MapLayer*>::const_iterator item_layer= data.layers.cbegin();
		while(item_layer != data.layers.end())
		{
			MapLayer* l = *item_layer;
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = next(item_layer);
		}
		//App->render->cam_limit_x = data.width * data.tile_width - App->render->camera.w;
		//App->render->cam_limit_y = data.height * data.tile_height - App->render->camera.h;
	}

	mapLoaded = ret;

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tileWidth = map.attribute("tilewidth").as_int();
		data.tileHeight = map.attribute("tileheight").as_int();
		eastl::string bg_color = map.attribute("backgroundcolor").as_string();

		data.backgroundColor.r = 0;
		data.backgroundColor.g = 0;
		data.backgroundColor.b = 0;
		data.backgroundColor.a = 0;

		if(bg_color.length() > 0)
		{
	
			std::string  red, green, blue;
			bg_color.substr(1, 2);
			bg_color.substr(3, 4);
			bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.backgroundColor.r = v;

			sscanf_s(green.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.backgroundColor.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.backgroundColor.b = v;
		}

		std::string orientation = map.attribute("orientation").as_string();

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name = tileset_node.attribute("name").as_string();
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tileWidth = tileset_node.attribute("tilewidth").as_int();
	set->tileHeight = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offsetX = offset.attribute("x").as_int();
		set->offsetY = offset.attribute("y").as_int();
	}
	else
	{
		set->offsetX = 0;
		set->offsetY = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.data(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->texWidth = image.attribute("width").as_int();

		if(set->texWidth <= 0)
		{
			set->texWidth = w;
		}

		set->texHeight = image.attribute("height").as_int();

		if(set->texHeight <= 0)
		{
			set->texHeight = h;
		}

		set->numTilesWidth = set->texWidth / set->tileWidth;
		set->numTilesHeight = set->texHeight / set->tileHeight;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = static_cast<uint>(tile.attribute("gid").as_int(0));
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if(data != NULL)
	{
		pugi::xml_node prop;

		for(prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.push_back(p);
		}
	}

	return ret;
}

bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	eastl::list<MapLayer*>::const_iterator item;
	item = data.layers.cbegin();

	for(; item != data.layers.end(); item = next(item))
	{
		MapLayer* layer = *item;

		if(layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;
				
				if(tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? (uchar)0 : (uchar)1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}