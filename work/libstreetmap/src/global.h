//
// Created by huan3409 on 3/22/24.
//

#ifndef MAPPER_GLOBAL_H
#define MAPPER_GLOBAL_H

#endif //MAPPER_GLOBAL_H

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"

//m1.cpp
#include <iostream>

#include "OSMDatabaseAPI.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>

//m2.cpp
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "gtk-3.0/gdk/gdkevents.h"
#include "ezgl/camera.hpp"
#include "gtk-2.0/gtk/gtkentry.h"
#include <sstream>
#include <unicode/ustring.h>

//m3.cpp
#include "list"
#include <queue>
#include <thread>

//m4.cpp
#include <chrono>
#define TIME_LIMIT 50
#include <functional>
#include "omp.h"
#include <unordered_set>

#include <cstdlib> // For std::rand and std::srand
#include <ctime>   // For std::ti
/*******************************declare vector*********************************/
//m1.cpp
extern double max_speed;
extern std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;
extern std::vector<std::vector<IntersectionIdx>> street_intersections;
extern std::vector<std::vector<StreetSegmentIdx>> street_segments;
extern std::vector<std::vector<StreetIdx>> intersection_street_id;
extern std::vector<const OSMNode*> NodeIndex_NodeId;
extern std::vector<OSMID> NodeIndex_OSMId;
extern std::vector<const OSMWay*> WayIndex_WayId;
extern std::vector<OSMID> WayIndex_OSMId;
extern std::vector<std::vector<LatLon>> Way_LatLon_of_Nodes;
extern std::vector<std::vector<double>> Way_WayLength;

//m2.cpp
//initialize the variables here
// struct to store intersection data
struct Intersection_data {
    LatLon position;
    std::string name;
    bool highlight = false;
};

//m3.cpp
//initialize the variables here
// struct to store find path data
struct WaveElem{
    IntersectionIdx nodeID;
    StreetSegmentIdx edgeID; //ID of edge used to reach NodeID
    double current_route_travel_time; //Total travel time to reach node
//    double my_idea_of_favourite_solution;
    double sort_time;
    WaveElem (int n, int e, float t1 , float t2) {nodeID = n; edgeID = e; current_route_travel_time = t1; sort_time = t2;}
};


struct CompareWaveElem {
    bool operator()(const WaveElem& lhs, const WaveElem& rhs) const {
        // Compare based on my_idea_of_favourite_solution
        return lhs.sort_time > rhs.sort_time;
    }
};

struct Node {
    std::vector<std::pair<StreetSegmentIdx, int>> out_going_edge_to_Node; //All edges connected to that node
    bool visited; //If the node is already visited for path search
    StreetSegmentIdx reaching_edge; //ID of Edge Used to reach this node
    double best_route_time;

};




extern std::vector<StreetSegmentIdx> popped;
extern std::vector<std::string> maps;
extern std::vector<Intersection_data> intersections;
extern std::vector<FeatureIdx> parks;
extern std::vector<FeatureIdx> lakes;
extern std::vector<FeatureIdx> rivers;
extern std::vector<FeatureIdx> beaches;
extern std::vector<FeatureIdx> islands;
extern std::vector<FeatureIdx> buildings;
extern std::vector<FeatureIdx> greenspaces;
extern std::vector<FeatureIdx> golfcourses;
extern std::vector<FeatureIdx> streams;
extern std::vector<FeatureIdx> glaciers;
extern std::vector<LatLon> subway_stations;
extern std::vector<std::pair<LatLon, std::string >> toilets;
extern std::vector<std::pair<LatLon, std::string >>  toilets_wheelchair;
extern std::vector<std::pair<LatLon, LatLon>> subway_nodes;
extern std::vector<std::pair<LatLon, LatLon>>  tertiary_highway_nodes;
extern std::vector<std::pair<LatLon, LatLon>>  secondary_highway_nodes;
extern std::vector<std::pair<LatLon, LatLon>>   primary_highway_nodes;
extern std::vector<std::pair<LatLon, LatLon>>    quaternary_highway_nodes;
extern std::vector<std::pair<LatLon, LatLon>>    primary_link_nodes;

extern ezgl::surface* surface_subway;
extern ezgl::surface* surface_toilet;
extern ezgl::surface* surface_toilet_wheelchair;
extern ezgl::surface* surface_location;

extern double avg_lat;
extern double max_lat;
extern double min_lat;
extern double max_lon;
extern double min_lon;

//m3.cpp
extern bool find_path_pressed;
extern std::vector<std::pair<StreetIdx, double>> segment_time;
extern std::vector<IntersectionIdx> highlighted;
extern std::vector<bool> segment_highlighted;
extern std::vector<bool> explore_highlighted;
extern std::vector<Node> Nodes;
extern std::vector<std::pair<StreetIdx, double >> Path_street_length;
extern std::vector<StreetSegmentIdx> make_vector_from_list(std::list<StreetSegmentIdx> list);
extern std::vector<StreetSegmentIdx> bfsTraceBack (int destID);
extern std::vector<std::string> turn_to;

/*******************************helper function*********************************/
//m1.cpp
void load_intersection_street_segments ();
void load_street_intersections ();
void load_streetIds_from_partialStreetName ();
void load_street_segments();
void load_intersection_street_id();
//Two helper functions for findAngleBetweenStreetSegments
double CosineLaw(double a, double b, double c);
// Helper Functions for findFeatureArea
double x_featureCoordinate (LatLon featurePoint, double lat_avg);
double y_featureCoordinate (LatLon featurePoint);
double computeFeatureArea (std:: vector<double> x_coord, std:: vector <double> y_coord, int numCoords);
//OSM helper Functions - Build OSM Data Base
void build_OSM_Database();
void buildNodeData();
void buildWayData();
void load_Way_LatLon_of_Nodes();
void load_Way_WayLength();
const OSMNode* getNodeById(const OSMID& nodeId);
const OSMWay* getWayById(const OSMID& wayId);
const OSMNode* matchingNodesForOSMID (OSMID osm_id);

//m2_pro
void set_map_database();
std::string get_map_address();
void set_map_address(std::string address);
bool get_input_status();
bool set_input_status(int num);

void initial_parks();
void initial_lakes();
void initial_rivers();
void initial_beaches();
void initial_islands();
void initial_buildings();
void initial_greenspaces();
void initial_golfcourses();
void initial_streams();
void initial_glaciers();

void load_max_min_lat_lon();
void init_subway_route();
void load_subway_png();
void load_toilet_png();
void load_toilet_wheelchair_png();
void load_location_png();

void SUBWAY_OSM_Database();
void toilet_OSM_Database();
void toilets_wheelchair_OSM_Database();

//m2.cpp
float x_from_lon(float lon);
float y_from_lat(float lat);
float lon_from_x(float x);
float lat_from_y(float y);

void draw_features( std::vector<FeatureIdx> FeatureName, ezgl:: renderer *g);
void draw_parks(ezgl:: renderer *g);
void draw_lakes(ezgl:: renderer *g);
void draw_rivers(ezgl:: renderer *g);
void draw_beaches(ezgl:: renderer *g);
void draw_islands(ezgl:: renderer *g);
void draw_buildings(ezgl:: renderer *g);
void draw_greenspaces(ezgl:: renderer *g);
void draw_golfcourses(ezgl:: renderer *g);
void draw_glaciers(ezgl:: renderer *g);
void draw_streams(ezgl:: renderer *g);
void draw_intersections (ezgl:: renderer *g);
void draw_subway_station(ezgl:: renderer *g);
void draw_toilet (ezgl::renderer* g);
void draw_toilet_label (ezgl::renderer* g);
void draw_toilets_wheelchair (ezgl::renderer* g);
void draw_toilets_wheelchair_label (ezgl::renderer* g);
void draw_scale(ezgl::renderer *g, double map_scale);
void draw_street_segments(ezgl::renderer* g, double scale);
void draw_subway_routes(ezgl::renderer*);

void initial_intersections();
bool street_contain_special_char(std::string);
double distance_scaling_fct (ezgl::renderer *g);
int scale_ratio (double map_scale);
void draw_main_canvas_xy_fixed_world (ezgl:: renderer *g);
void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);
void find_intersections(GtkWidget* /*widget*/, ezgl::application* application);
void take_map_input(GtkWidget* /*widget*/, ezgl::application* application);
void initial_setup(ezgl::application* application, bool /*new_window*/);
void textEntry_enter (GtkEntry* textEntryWidget, ezgl::application* application);
gboolean pinkify_switch (GtkSwitch* /*pinkify_switch*/, gboolean switch_state, ezgl::application* application);
void initial_setup_pinkify_switch_with_text (ezgl::application* application, bool /*new window*/);
void draw_map_intersections_xy_fixed_world();
void toogle_pink(GtkWidget* /*widget*/, ezgl::application* application);
void draw_street_name (ezgl::renderer* g, ezgl::point2d start, ezgl::point2d end, int index, int size);
gboolean show_subways (GtkSwitch* /*subways_switch*/, gboolean switch_state, ezgl::application* application);

//m3.cpp
void load_segment_time();
void clear_highlight(GtkWidget* /*widget*/, ezgl::application* application);
void find_path (GtkWidget* /*widget*/, ezgl::application* application);
void initial_segment_highlighted();
void help_information(GtkWidget* /*widget*/, ezgl::application* application);
void init_nodes();
void find_direction(std::vector<StreetSegmentIdx> path);
void draw_seg_using_seg_id(StreetSegmentIdx i, ezgl::renderer* g);
void display_direction(GtkWidget* /*widget*/, ezgl::application* application);
bool isInt(const std::string& str);
double travel_time(StreetSegmentIdx last_edge, StreetSegmentIdx next_edge, double turn_penalty);
double find_fastest_possible_time(IntersectionIdx my_location, IntersectionIdx destination, double total_travel_time_so_far);
bool bfs_path(int srcID, int destID, double turn_penalty);
std::string find_turn_to(StreetSegmentIdx first, StreetSegmentIdx second);
void display_road_length();
int roundToNearestHundred(double num);
void find_total_time(std::vector<StreetSegmentIdx> path);


