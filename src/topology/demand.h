#ifndef __Demand__h
#define __Demand__h

#include <string>
#include <iostream>

#include <vector>

/********************************************************************************************
 * This class identifies a demand. A demand is defined by its id, its source and target node, 
 * its load (i.e., how many slices it requests) and its maximal length (i.e., how long can be
 * its routing path). It also can be already routed or not. \note The id of each Demand is 
 * considered to be in the range [0, ..., n-1].
 ********************************************************************************************/
class Demand
{
private:
	int id;				/**< The demand's id. **/
	int source;			/**< The demand's source node id. **/
	int target;			/**< The demand's target node id. **/
	int loadC;			/**< Refers to how many slices the demand requires. **/
	int loadL;			/**< Refers to how many slices the demand requires. **/
	double maxCDC;	/**< Refers to the maximum length of the path on which the demand can be routed. **/
	double osnrLimitC;	/**< Refers to the minimum OSNR of the path on which the demand can be routed. **/
	double maxCDL;	/**< Refers to the maximum length of the path on which the demand can be routed. **/
	double osnrLimitL;	/**< Refers to the minimum OSNR of the path on which the demand can be routed. **/
	double maxCDS;	/**< Refers to the maximum length of the path on which the demand can be routed. **/
	double osnrLimitS;	/**< Refers to the minimum OSNR of the path on which the demand can be routed. **/

	double gBits;	
	int transpIdC;	
	int transpIdL;	

	bool routed;		/**< True if the demand is already routed. **/
	int sliceAllocation;/**< The id of the last slice assigned to this demand. @warning Equals -1 if not routed yet. **/
	double pathLength;	/**< The length of the path on which demand is routed. @warning Equals -1 if not routed yet. **/
	int nbHops;			/**< The number of hops of the path on which demand is routed. @warning Equals -1 if not routed yet. **/
	
	double pchC;			/**< Refers to the power of the channel of the demand **/
	double pchL;			/**< Refers to the power of the channel of the demand **/
	double pchS;

	std::string mode;	/**< The GNPY mode of transmission. **/
	std::string spacing;/**< The GNPY spacing. **/
	std::string band;	/**< The GNPY path_bandwidth. **/

public:
	/****************************************************************************************/
	/*									Constructors										*/
	/****************************************************************************************/
	/** Constructor. @param i Id. @param s Source node id. @param t Target node id. @param l Load. 
	 * @param maxL Maximum length. @param osL Minimun OSNR. @param p Power of the channel. @param pc Power of the channel. @param pl Power of the channel. @param route Whether the demand is already routed. 	
	 * @param pos The last slice position assigned to the demand. @param len The length of the path assigned to the demand. 
	 * @param hop The number of hops in the path assigned to the demand. * @param m The GNPY mode @param space The GNPY spacing 
	 * @param pathBand The GNPY path_bandwidth. **/
	Demand(int i = -1, int s = -1, int t = -1, int lC = 0, int lL = 0, double maxC = 0, double maxL = 0, double osC = 1, double osL = 0, int tC = 0, int tL = 0, double pc = 0,double pl = 0, double ps = 1, double gb = 0.0, bool route=false, int pos=-1, double len = 0, int hop = 0, std::string m="", std::string space="",std::string pathBand="");

	/****************************************************************************************/
	/*										Getters											*/
	/****************************************************************************************/
	/** Returns the demand's id. **/
	int getId() const { return id; }
	
	/** Returns the demand's source node id. **/
	int getSource() const { return source; }

	/** Returns the demand's target node id. **/
	int getTarget() const { return target; }
	
	/** Returns the number of slices requested by the demand. **/
	int getLoadC() const { return loadC; }

	/** Returns the number of slices requested by the demand. **/
	int getLoadL() const { return loadL; }
	

	int getTranspIdC() const { return transpIdC; }
	int getTranspIdL() const { return transpIdL; }

	/** Returns the id of the last slice assigned to the demand. **/
	int getSliceAllocation() const { return sliceAllocation; }

	/** Returns the length of the path on which demand is routed. **/
	double getPathLength() const { return pathLength; }

	/** Returns the number of hops of the path on which demand is routed. **/
	int getNbHops() const { return nbHops; }
	
	/** Returns the maximum length of the path on which the demand can be routed. **/
	double getmaxCDC() const { return maxCDC; }

	/** Returns the maximum length of the path on which the demand can be routed. **/
	double getmaxCDL() const { return maxCDL; }

	/** Returns the maximum length of the path on which the demand can be routed. **/
	double getmaxCDS() const { return maxCDS; }

	/** Returns the minimum OSNR of the path on which the demand can be routed. **/
	double getOsnrLimitC() const { return osnrLimitC; }
	/** Returns the minimum OSNR of the path on which the demand can be routed. **/
	double getOsnrLimitL() const { return osnrLimitL; }
	/** Returns the minimum OSNR of the path on which the demand can be routed. **/
	double getOsnrLimitS() const { return osnrLimitS; }


	double getGBits() const { return gBits; }

	/** Returns the power of the channel of the demand **/
	double getPchC() const { return pchC; }
	double getPchL() const { return pchL; }
	double getPchS() const { return pchS; }

	/** Returns true if the demand has already been routed. **/
	bool isRouted() const { return routed; }

	/** Returns the GNPY mode. **/
	std::string getMode() const { return mode; }

	/** Returns the GNPY spacing. **/
	std::string getSpacing() const { return spacing; };

	/** Returns the GNPY path_bandwidth. **/
	std::string getPathBandwidth() const { return band; };

	/** Returns a compact description of the demand in the form (source, target, load). **/
	std::string getString() const;

	/****************************************************************************************/
	/*										Setters											*/
	/****************************************************************************************/
	/** Changes the demand's id. @param i New id value. **/
	void setId(int i) { this->id = i; }
	
	/** Changes the demand's source node id. @param s New source node id. **/
	void setSource(int s) { this->source = s; }
	
	/** Changes the demand's target node id. @param t New target node id. **/
	void setTarget(int t) { this->target = t; }
	
	/** Changes the number of slices requested by the demand. @param l New load value.**/
	void setLoadC(int l) { this->loadC = l; }

	/** Changes the number of slices requested by the demand. @param l New load value.**/
	void setLoadL(int l) { this->loadL = l; }


	void setTranspIdC(int t) { this->transpIdC = t; }
	void setTranspIdL(int t) { this->transpIdL = t; }

	/** Changes the id of the last slice assigned to the demand. @param i The new last slice position.**/
	void setSliceAllocation(int i) { this->sliceAllocation = i; }
	
	/** Changes the maximum length of the path on which the demand can be routed. @param max New demand's maximum length value.**/
	void setmaxCDC(double max) { this->maxCDC = max; }

	/** Changes the maximum length of the path on which the demand can be routed. @param max New demand's maximum length value.**/
	void setmaxCDL(double max) { this->maxCDL = max; }

	/** Changes the OSNR Limit of the path on which demand is routed. @param osL The new path Osnr Limit. **/
	void setOsnrLimitC(double os) { this->osnrLimitC = os; }

		/** Changes the OSNR Limit of the path on which demand is routed. @param osL The new path Osnr Limit. **/
	void setOsnrLimitL(double os) { this->osnrLimitL = os; }

	/** Changes the maximum length of the path on which the demand can be routed. @param max New demand's maximum length value.**/
	void setmaxCDS(double max) { this->maxCDS = max; }

	/** Changes the OSNR Limit of the path on which demand is routed. @param osL The new path Osnr Limit. **/
	void setOsnrLimitS(double os) { this->osnrLimitS = os; }


	void setGBits(double gb) { this->gBits = gb; }

	/** Changes the power of the channel of the demand **/
	void setPchC(double p) { this->pchC = p; }
	void setPchL(double p) { this->pchL = p; }
	void setPchS(double p) { this->pchS = p; }

	/** Changes the status of a demand. @param b If true, demand becomes routed. Otherwise, it becomes non-routed.**/
	void setRouted(bool b) { this->routed = b; }
	
	/** Changes the length of the path on which demand is routed. @param l The new path length. **/
	void setPathLength(double l) { this->pathLength = l; }

	/** Changes the number of hops of the path on which demand is routed. @param n The new number of hops**/
	void setNbHops(int n) { this->nbHops = n; }

	/** Changes the GNPY mode. @param m The new mode**/
	void setMode(std::string m) { this->mode = m; }

	/** Changes the GNPY spacing. @param s The new spacing. **/
	void setSpacing(std::string s) { this->spacing = s; }

	/** Changes the GNPY path bandwidth. @param b The new path bandwidth. **/
	void setPathBandwidth(std::string b) { this->band = b; }


	/****************************************************************************************/
	/*										Methods											*/
	/****************************************************************************************/
	/** Copies all information from a given demand. @param demand The demand to be copied. **/
	void copyDemand(const Demand &demand);

	/** Verifies if the demand has exactly the given informations. @param id The supposed id value. @param source The supposed source node id value. @param target The supposed target node id value. @param load The supposed load value.**/
	void checkDemand(int id, int source, int target, int load);

	/****************************************************************************************/
	/*										Display											*/
	/****************************************************************************************/
	/** Displays demand information. **/
	void displayDemand();
};

#endif