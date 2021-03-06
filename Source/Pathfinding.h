#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"
#include "p2Point.h"
#include "EASTL/vector.h"
#include "EASTL/list.h"
#include "PerfTimer.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

class Ai;

class PathFinding : public Module
{
public:

	PathFinding();

	// Destructor
	~PathFinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B
	int CreatePath(const iPoint& origin, const iPoint& destination);

	// To request all tiles involved in the last generated path
	const eastl::vector<iPoint>* GetLastPath() const;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	bool IsWalkable(const iPoint& pos, uint size) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	//TODO: Find the path when requested from an ai group (calcular un path mitja per a tots)
	bool CalculateGroupPath(eastl::list<Ai*>group,iPoint destination);

	bool AssignGoalTile(iPoint tile,Ai*ai);

	bool IsOccupied(iPoint tile);

	eastl::vector<iPoint> FindWalkableAdjacents(iPoint currentpoint);

	//TODO: Update	walkability map based on units position and add it to the pathfinding algorithm
	//bool IsTileOccupied();
	//UpdateUnitsLocations();
	//TODO: Find path request manager
private:

	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	uchar* map;
	// we store the created path here
	//TODO:Llista de tots els pathos
	eastl::list<eastl::vector<iPoint>> currentPaths;
	eastl::vector<iPoint> lastPath;
	eastl::list<iPoint> occupiedTiles;
	PerfTimer timer;
};

// forward declaration
struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill) const;
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	int OctileDistance(const iPoint& destination);

	bool operator>(const PathNode& node) const{
		return Score() > node.Score();
	}
	bool operator<(const PathNode& node) const {
		return Score() < node.Score();
	}
	bool operator==(const PathNode& node) {
		return pos == node.pos;
	}

	// -----------
	int g;
	int h;
	iPoint pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	const PathNode* Find(const iPoint& point) const;

	//TODO: Implement the heap to find the lowest node
	// Returns the Pathnode with lowest score in this list or NULL if empty
	const PathNode* GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	eastl::list<PathNode> list;
};
#endif // __PATHFINDING_H__