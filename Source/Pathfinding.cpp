#include "p2Defs.h"
#include "p2Log.h"
#include "Application.h"
#include "PathFinding.h"
#include "Ai.h"

PathFinding::PathFinding() : Module(), map(NULL), lastPath(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name = "pathfinding";
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");
	occupiedTiles.clear();
	lastPath.clear();
	RELEASE_ARRAY(map);
	return true;
}

eastl::vector<iPoint> PathFinding::FindWalkableAdjacents(iPoint currentpoint) {
	iPoint cell;
	eastl::vector<iPoint>adjacent;
	//diagonals
	cell.create(currentpoint.x + 1, currentpoint.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	cell.create(currentpoint.x + 1, currentpoint.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	cell.create(currentpoint.x - 1, currentpoint.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	cell.create(currentpoint.x - 1, currentpoint.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	// north
	cell.create(currentpoint.x, currentpoint.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	// south
	cell.create(currentpoint.x, currentpoint.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	// east
	cell.create(currentpoint.x + 1, currentpoint.y);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	// west
	cell.create(currentpoint.x - 1, currentpoint.y);
	if (App->pathfinding->IsWalkable(cell))
		adjacent.push_back(cell);

	return adjacent;
}

// Sets up the walkability map
void PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

bool PathFinding::IsWalkable(const iPoint& pos, uint size) const{
	bool ret = true;
	iPoint cell = pos;
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			cell.x+= i;
			cell.y+= j;
			uchar t = GetTileAt(cell);
			ret = t != INVALID_WALK_CODE && t > 0;
			if (!ret)
				return false;
		}
	}
	return true;
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const eastl::vector<iPoint>* PathFinding::GetLastPath() const
{
	return &lastPath;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const PathNode* PathList::Find(const iPoint& point) const
{
	eastl::list<PathNode>::const_iterator item = list.begin();

	while (item != list.end())
	{
		if ((*item).pos == point)
			return &(*item);
		item++;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
const PathNode* PathList::GetNodeLowestScore() const
{
	const PathNode* ret = NULL;
	int min = 65535;

	eastl::list <PathNode>::const_reverse_iterator item = list.rbegin();

	while (item != list.rend())
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = &(*item);
		}
		item++;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	//diagonals
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// north
	cell.create(pos.x, pos.y + 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

int PathNode::OctileDistance(const iPoint& destination) {
	int dx = abs(pos.x - destination.x);
	int dy = abs(pos.y - destination.y);
	/*
	if (dx > dy)
		(32 * (dx - dy) + sqrt(32 * 32 + 32 * 32) * dy);
	else
		(32 * (dy - dx) + sqrt(32 * 32 + 32 * 32) * dx);
	*/
	//TODO: Investigar mes sobre aquesta heuristica (utilitzar la de 32 o 1 i arrel de 2?Es millor que utilitzar la heuristca de la distancia euclideana?)
	if (dx > dy)
		return (1 * (dx - dy) + sqrt(2) * dy);
	else
		return (1 * (dy - dx) + sqrt(2) * dx);
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	PERF_START(timer);
	int ret = -1;
	//if origin or destination are not walkable, return -1
	if (!(IsWalkable(origin) && IsWalkable(destination) && origin!=destination))
		return ret;
	//Create two lists: open, close
	// Add the origin tile to open
	// Iterate while we have tile in the open list
	PathList open;
	PathList closed;
	open.list.push_back(PathNode(0,0,origin,nullptr));
	while (open.list.size() > 0) {
	//Move the lowest score cell from open list to the closed list
		PathNode* lowest = (PathNode*) open.GetNodeLowestScore();
		closed.list.push_back(*lowest);
		open.list.erase(eastl::find(open.list.begin(), open.list.end(), *lowest));

	//If we just added the destination, we are done!
	// Backtrack to create the final path
	// Use the Pathnode::parent and Flip() the path when you are finish
		if (closed.list.back().pos == destination) {
			lastPath.clear();
			lastPath.shrink_to_fit();
			const PathNode* path_node = &closed.list.back();
			while (path_node) {
				lastPath.push_back(path_node->pos);
				path_node = path_node->parent;
			}
			eastl::reverse(lastPath.begin(), lastPath.end());
			ret = lastPath.size();
			break;
		}
	//Fill a list of all adjancent nodes
		PathList adjacent;
		closed.list.back().FindWalkableAdjacents(adjacent);
	//Iterate adjancent nodes:
		//p2List_item<PathNode>*item = adjacent.list.start;
		eastl::list<PathNode>::iterator item = adjacent.list.begin();
		for (; item != adjacent.list.end(); item++){
	        // ignore nodes in the closed list
			if (closed.Find((*item).pos)!=NULL)
				continue;
			// If it is NOT found, calculate its F and add it to the open list
			if (open.Find((*item).pos) == NULL) {
				(*item).CalculateF(destination);
				open.list.push_back((*item));
			}
			else {
				// If it is already in the open list, check if it is a better path (compare G)
				// If it is a better path, Update the parent
				PathNode adjacent_in_open = *open.Find((*item).pos);
				// --- We compare walkable cost and if the neighbor's is bigger than current + offset we know this is a better path and update neighbor's parent and F ---
				//if (adjacent_in_open.diagonal == false)
				if (adjacent_in_open.g > (*item).g + 1){
					adjacent_in_open.parent = (*item).parent;
					adjacent_in_open.CalculateF(destination);
				}
			}
		}
	}
	PERF_PEEK(timer);
	return ret;
}


/*			// If it is NOT found, calculate its F and add it to the open list
			if (open.Find((*item).pos) == NULL) {
				(*item).CalculateF(destination);
				open.list.push_back((*item));
			}
			else {
				// If it is already in the open list, check if it is a better path (compare G)
				// If it is a better path, Update the parent
				PathNode adjacent_in_open = *open.Find((*item).pos);
				// --- We compare walkable cost and if the neighbor's is bigger than current + offset we know this is a better path and update neighbor's parent and F ---
				//if (adjacent_in_open.diagonal == false)
				if (adjacent_in_open.g > (*item).g + 1){
					adjacent_in_open.parent = (*item).parent;
					adjacent_in_open.CalculateF(destination);
				}
			}
		}*/
bool PathFinding::IsOccupied(iPoint tile) {
	return eastl::find(occupiedTiles.begin(), occupiedTiles.end(), tile) != occupiedTiles.end();
}

bool PathFinding::CalculateGroupPath(eastl::list<Ai*>group,iPoint destination) {
	PERF_START(timer);
	eastl::list<Ai*>::iterator it;
	iPoint AveragePos(0,0);
	for (it = group.begin(); it != group.end(); ++it) {
		AveragePos += (*it)->TilePos;
	}
	AveragePos = iPoint(AveragePos.x/group.size(), AveragePos.y / group.size());
	if (CreatePath(AveragePos, destination) != -1) {
		if (GetLastPath()->size() > 10) {
			eastl::vector<iPoint> ShearedPath = *GetLastPath();
			eastl::list<Ai*>::iterator item;
			for (item = group.begin(); item != group.end(); ++item) {
				(*item)->OnDestination = false;
				CreatePath((*item)->TilePos, ShearedPath[10]);
				(*item)->path = *GetLastPath();
				(*item)->path.insert((*item)->path.end(), ShearedPath.begin() + 10, ShearedPath.end());
				eastl::list<Ai*>::const_iterator cit;
				for (cit = group.cbegin(); cit != group.cend(); ++cit) {
					if (AssignGoalTile(*((*cit)->path.end() - 1), (*item)))
						break;
				}
			}
			occupiedTiles.clear();
		}
		else {
			return false;
		}
	}
	PERF_PEEK(timer);
	return true;
}

bool PathFinding::AssignGoalTile(iPoint tile,Ai*ai) {
	iPoint cell;

	// north
	cell.create(tile.x, tile.y + 1);
	if (App->pathfinding->IsWalkable(cell)&& !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	// south
	cell.create(tile.x, tile.y - 1);
	if (App->pathfinding->IsWalkable(cell)&& !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	// east
	cell.create(tile.x + 1, tile.y);
	if (App->pathfinding->IsWalkable(cell) && !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	// west
	cell.create(tile.x - 1, tile.y);
	if (App->pathfinding->IsWalkable(cell) && !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	//diagonals
	cell.create(tile.x + 1, tile.y + 1);
	if (App->pathfinding->IsWalkable(cell) && !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	cell.create(tile.x + 1, tile.y - 1);
	if (App->pathfinding->IsWalkable(cell) && !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	cell.create(tile.x - 1, tile.y + 1);
	if (App->pathfinding->IsWalkable(cell) && !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	cell.create(tile.x - 1, tile.y - 1);
	if (App->pathfinding->IsWalkable(cell) && !IsOccupied(cell)) {
		ai->path.erase(ai->path.end() - 10, ai->path.end());
		CreatePath(ai->path.back(), cell);
		eastl::vector<iPoint>endingpath = *GetLastPath();
		ai->path.insert(ai->path.end(), endingpath.begin(), endingpath.end());
		occupiedTiles.push_back(cell);
		return true;
	}

	return false;
}