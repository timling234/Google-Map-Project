/* 
 * Copyright 2024 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "global.h"
//initial global
int total_distance;
int total_time;

std::vector<IntersectionIdx> highlighted;
std::vector<bool> segment_highlighted;
std::vector<StreetSegmentIdx> popped;
std::vector<bool> explore_highlighted;
std::vector<std::pair<StreetIdx, double >> Path_street_length;
std::vector<std::string> turn_to;
std::vector<std::pair<LatLon, LatLon>> tertiary_highway_nodes;
std::vector<std::pair<LatLon, LatLon>> secondary_highway_nodes;
std::vector<std::pair<LatLon, LatLon>> primary_highway_nodes;
std::vector<std::pair<LatLon, LatLon>> quaternary_highway_nodes;
std::vector<std::pair<LatLon, LatLon>> primary_link_nodes;
std::vector<int> path_length;

// big scope variables in program

bool pinkify = false;
bool subway_show = false;
bool find_path_pressed = false;

// main draw map call return to main
void drawMap() {
    // Set up the ezgl graphics window and hand control to it, as shown in the
    // ezgl example program.
    // This function will be called by both the unit tests (ece297exercise)
    // and your main() function in main/src/main.cpp.
    // The unit tests always call loadMap() before calling this function
    // and call closeMap() after this function returns.

    draw_map_intersections_xy_fixed_world();
}

// ****** Helper Functions Here ****** //


//Converting Lat/Lon to Cartesian X/Y
float x_from_lon(float lon) {
    return (lon * kDegreeToRadian * kEarthRadiusInMeters * std::cos(avg_lat * kDegreeToRadian));
}

float y_from_lat(float lat) {
    return kEarthRadiusInMeters * lat * kDegreeToRadian;
}

// converting X/Y from Lat/Lon
float lon_from_x(float x) {
    return (x/(kDegreeToRadian * kEarthRadiusInMeters * std::cos(avg_lat * kDegreeToRadian)));
}

float lat_from_y(float y) {
    return (y/(kEarthRadiusInMeters * kDegreeToRadian));
}


//draw all features here, classify closed polygon, line and point
void draw_features( std::vector<FeatureIdx> FeatureName, ezgl:: renderer *g){

    const ezgl::point2d top_right(g->get_visible_world().top_right());
    const ezgl::point2d bottom_left(g->get_visible_world().bottom_left());


//    std::cout<<bottom_left.x<<" "<<bottom_left.y<<std::endl;
//    std::cout<<top_right.x<<" "<<top_right.y<<std::endl;


    //Closed Polygon
    for(int i=0; i < FeatureName.size(); ++i){
        std::vector<ezgl::point2d> polygon;
        bool poly_in_world = false;
        //Each feature
        for (int j=0; j<getNumFeaturePoints(FeatureName[i]); ++j){
            //Each feature in the Lake Vector
            LatLon pos = getFeaturePoint(j,FeatureName[i]);
            ezgl::point2d xy_pos= {x_from_lon(pos.longitude()), y_from_lat(pos.latitude())};


            if (( (xy_pos.x>bottom_left.x) && (xy_pos.x<top_right.x) && (xy_pos.y>bottom_left.y) && (xy_pos.y<top_right.y) )){
                poly_in_world = true;
            }


            polygon.push_back(xy_pos);
        }

        if(polygon.size()<=1){
            g->fill_rectangle(polygon[0], 5, 5);
            continue;
            //Size !> 1
        }

        if(polygon[0] != polygon[polygon.size()-1]){
            g->draw_line(polygon[0], polygon[1]);
            continue;
            //Not closed - Line Feature
        }

        if(!poly_in_world){
            continue;
        }

        g->fill_poly(polygon);
    }

}


//set the color of each feature, including day_mode and dark_mode, then draw
void draw_parks(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(128, 210, 128);
    }
    if (pinkify) {
        g->set_color(63, 100, 67);
    }
    draw_features(parks, g);
}

void draw_lakes(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(143,190,209);
    }
    if (pinkify) {
        g->set_color(ezgl::BLACK);
    }
    draw_features(lakes, g);
}

void draw_rivers(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(ezgl::LIGHT_SKY_BLUE);
    }
    if (pinkify) {
        g->set_color(47, 69, 74);
    }
    draw_features(rivers, g);
}

void draw_beaches(ezgl:: renderer *g) {
    if (!pinkify) {
        g->set_color(235, 200, 130);
    }
    if (pinkify) {
        g->set_color(87, 77, 54);
    }
    draw_features(beaches, g);
}

void draw_islands(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(123, 205, 123);
    }
    if (pinkify) {
        g->set_color(63, 100, 67);
    }
    draw_features(islands, g);
}

void draw_buildings(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(190,190, 166);
    }
    if (pinkify) {
        g->set_color(61,48,49);
    }
    draw_features(buildings, g);
}

void draw_greenspaces(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(120, 210, 120);
    }
    if (pinkify) {
        g->set_color(73, 100, 77);
    }
    draw_features(greenspaces, g);
}

void draw_golfcourses(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(ezgl::GREY_75);
    }
    if (pinkify) {
        g->set_color(56, 60, 64);
    }
    draw_features(golfcourses, g);
}
void draw_glaciers(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(ezgl::BLUE);
    }
    if (pinkify) {
        g->set_color(ezgl::BLACK);
    }
    draw_features(glaciers, g);
}

void draw_streams(ezgl:: renderer *g){
    if (!pinkify) {
        g->set_color(135,206,250);
    }
    if (pinkify) {
        g->set_color(ezgl::BLACK);
    }
    draw_features(streams, g);
}

//draw all intersections in the map
void draw_intersections (ezgl:: renderer *g){

    //get all the intersections
    for (size_t i = 0; i < intersections.size(); ++i){

        float x = x_from_lon(intersections[i].position.longitude());
        float y = y_from_lat(intersections[i].position.latitude());

        // if highlighted, show the location icon
        if(intersections[i].highlight){
            g ->draw_surface(surface_location,{x,y}, 0.065);

            if (!pinkify) {
                g->set_color(ezgl::BLACK);
            }
            if (pinkify){
                g -> set_color (ezgl::WHITE);
            }

            std::string name = intersections[i].name;
            size_t pos = name.find("<unknown>");

            if(pos == -1){

                g->draw_text({x, y}, name, 100000, 1000000);
                continue;
            }

            name.replace(pos,name.size(),"Location");
            g->draw_text({x, y}, name, 100000, 1000000);
        }

    }
}

//draw all the vector related to the OSM Database
void draw_subway_station(ezgl:: renderer *g){
    for(int i = 0; i < subway_stations.size(); ++i) {
        double x = x_from_lon(subway_stations[i].longitude());
        double y = y_from_lat(subway_stations[i].latitude());
        g ->draw_surface(surface_subway,{x,y}, 0.065);
    }
}
void draw_toilet (ezgl::renderer* g){
    for(int i = 0; i < toilets.size(); ++i) {
        double x = x_from_lon(toilets[i].first.longitude());
        double y = y_from_lat(toilets[i].first.latitude());
        g ->draw_surface(surface_toilet,{x,y}, 0.061);
//        if (scale >1.435)
        g->set_color(ezgl::BLACK);
        g->set_font_size(12);
    }
}


void draw_toilet_label (ezgl::renderer* g) {
    for (int i = 0; i < toilets.size(); ++i) {
        double x = x_from_lon(toilets[i].first.longitude());
        double y = y_from_lat(toilets[i].first.latitude());
//        if (scale >1.435)
        g->set_color(ezgl::BLACK);
        g->set_font_size(12);
        g->draw_text({x, y - 4}, toilets[i].second, 100000, 1000000);
    }
}

void draw_toilets_wheelchair (ezgl::renderer* g){
    for(int i = 0; i < toilets_wheelchair.size(); ++i) {
        double x = x_from_lon(toilets_wheelchair[i].first.longitude());
        double y = y_from_lat(toilets_wheelchair[i].first.latitude());
        g ->draw_surface(surface_toilet_wheelchair,{x,y}, 0.019);
    }
}

void draw_toilets_wheelchair_label (ezgl::renderer* g){
    for(int i = 0; i < toilets_wheelchair.size(); ++i) {
        double x = x_from_lon(toilets_wheelchair[i].first.longitude());
        double y = y_from_lat(toilets_wheelchair[i].first.latitude());
        g->set_color(ezgl::BLACK);
        g->set_font_size(12);
        g->draw_text({x,y-4},toilets_wheelchair[i].second, 100000, 1000000);
    }
}


// helper function helps to calculate the ratio of zooming
double distance_scaling_fct (ezgl::renderer *g) {
    double original_scaling = (g-> get_visible_world()).width();
    double current_scaling = (g -> get_visible_screen()).width();

    double scale = current_scaling/original_scaling;
    return scale;
}

// helper function helps to display scale on map ui
int scale_ratio (double map_scale) {
    // display scale label
    double difference_map_width = x_from_lon(max_lon) - x_from_lon(min_lon);
    int ratio = (int) (map_scale * difference_map_width);
    return ratio;
}


// function draws street names on map
void draw_street_name (ezgl::renderer* g, ezgl::point2d start, ezgl::point2d end, int index, int size) {

    double center_text_x = (start.x + end.x)/2.0;
    double center_text_y = (start.y + end.y)/2.0;

    const ezgl::point2d top_right(g->get_visible_world().top_right());
    const ezgl::point2d bottom_left(g->get_visible_world().bottom_left());

    if (!((center_text_x>bottom_left.x) && (center_text_x<top_right.x) && (center_text_y>bottom_left.y) && (center_text_y<top_right.y) )){
        return;
    }
    g->set_color(ezgl::BLACK);

    StreetIdx street_id = getStreetSegmentInfo(index).streetID;
    std::string street_display_name = getStreetName(street_id);
    if ((street_display_name != "<unknown>") && (start != end) && ((size > 3) && (index % 2 == 0))) {

        ezgl::point2d text_location = {center_text_x, center_text_y};

        double angle_of_text = atan2((end.y - start.y), (end.x - start.x))/kDegreeToRadian;
        if (angle_of_text < 0) {
            angle_of_text += 180; // add pi
        }
        double segment_length = sqrt(pow((end.y - start.y), 2) + pow((end.x - start.x), 2));
        if ((sizeof(street_display_name) < segment_length)) {
            g->set_text_rotation(angle_of_text);
            g-> draw_text (text_location, street_display_name);
        }
        // make other texts normal rotation
        g->set_text_rotation(0);
    }

    g->set_color(202,202,202);
}


void draw_seg_using_seg_id(StreetSegmentIdx i, ezgl::renderer* g){
    int numCurvesInSeg = getStreetSegmentInfo(i).numCurvePoints;
    LatLon start_lat_lon = getIntersectionPosition(getStreetSegmentInfo(i).from);
    LatLon to_lat_lon = getIntersectionPosition(getStreetSegmentInfo(i).to);
    ezgl::point2d start = {x_from_lon(start_lat_lon.longitude()), y_from_lat(start_lat_lon.latitude())};
    ezgl::point2d end = {x_from_lon(to_lat_lon.longitude()), y_from_lat(to_lat_lon.latitude())};
    // straight line and curves
    if (numCurvesInSeg != 0) {

        ezgl::point2d prev = start;
        for (int j = 0; j < numCurvesInSeg; ++j) {
            LatLon point = getStreetSegmentCurvePoint(j, i);
            ezgl::point2d next = {x_from_lon(point.longitude()), y_from_lat(point.latitude())};
            g->draw_line(prev, next);
            prev = next;
        }
        g->draw_line(prev, end);
    } else {
        g->draw_line(start, end);
    }
}
// function draw all street segments of map at various scales
void draw_street_segments(ezgl::renderer* g, double scale){
    g->set_line_width(2);
    if(scale >0.08){
        for(int i=0; i<quaternary_highway_nodes.size(); ++i){
            LatLon from = quaternary_highway_nodes[i].first;
            LatLon to = quaternary_highway_nodes[i].second;
            ezgl::point2d from_from = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
            ezgl::point2d to_to = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};
            if(!pinkify){
                g->set_color(202,202,202);
            }else{
                g->set_color(98,111,138);
            }

            g->draw_line(from_from, to_to);

        }
    }




    if(scale > 0.2){
        if(!pinkify){
            g->set_color(202,202,202);
        }else{
            g->set_color(98,111,138);
        }
        for(int i=0; i<getNumStreetSegments(); ++i){
            float street_seg_speed = 3.6 * (getStreetSegmentInfo(i).speedLimit);
            if(street_seg_speed >90){
                continue;
            }
            LatLon from = getIntersectionPosition(getStreetSegmentInfo(i).from);
            LatLon to = getIntersectionPosition(getStreetSegmentInfo(i).to);
            ezgl::point2d start = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
            ezgl::point2d end = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};
            //This shows explored region, this section is for testing only
            int numCurvesInSeg = getStreetSegmentInfo(i).numCurvePoints;
            // straight line and curves
            if (numCurvesInSeg != 0) {

                ezgl::point2d prev = start;
                for (int j = 0; j < numCurvesInSeg; ++j) {
                    LatLon point = getStreetSegmentCurvePoint(j, i);
                    ezgl::point2d next = {x_from_lon(point.longitude()), y_from_lat(point.latitude())};
                    g->draw_line(prev, next);
                    prev = next;
                }
                g->draw_line(prev, end);
            } else {
                g->draw_line(start, end);
            }


        }
    }

    if(scale >0.08){
        for(int i=0; i<tertiary_highway_nodes.size(); ++i){
            LatLon from = tertiary_highway_nodes[i].first;
            LatLon to = tertiary_highway_nodes[i].second;
            ezgl::point2d from_from = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
            ezgl::point2d to_to = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};
            if(!pinkify){
                g->set_color(202,202,202);
            }else{
                g->set_color(98,111,138);
            }

            g->draw_line(from_from, to_to);



        }
    }
    if(scale >0.024){
        for(int i=0; i<secondary_highway_nodes.size(); ++i){
            LatLon from = secondary_highway_nodes[i].first;
            LatLon to = secondary_highway_nodes[i].second;
            ezgl::point2d from_from = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
            ezgl::point2d to_to = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};

            if(!pinkify){
                g->set_color(202,202,202);
            }else{
                g->set_color(108,121,148);
            }

            g->draw_line(from_from, to_to);
        }
    }

    for(int i=0; i<primary_link_nodes.size(); ++i){
        LatLon from = primary_link_nodes[i].first;
        LatLon to = primary_link_nodes[i].second;
        ezgl::point2d from_from = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
        ezgl::point2d to_to = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};

        if(!pinkify){
            g->set_color(208,208,208);
        }else{
            g->set_color(108,121,148);
        }


        g->draw_line(from_from, to_to);
    }


    for(int i=0; i<primary_highway_nodes.size(); ++i){
        LatLon from = primary_highway_nodes[i].first;
        LatLon to = primary_highway_nodes[i].second;
        ezgl::point2d from_from = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
        ezgl::point2d to_to = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};

        if(!pinkify){
            g->set_color(243,180,46);
        }else{
            g->set_color(108,121,148);
        }

        g->draw_line(from_from, to_to);

    }





    if(scale > 2.5){
        for(int i=0; i<getNumStreetSegments(); ++i){
            LatLon from = getIntersectionPosition(getStreetSegmentInfo(i).from);
            LatLon to = getIntersectionPosition(getStreetSegmentInfo(i).to);
            ezgl::point2d start = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
            ezgl::point2d end = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};

            if (pinkify) {

                draw_street_name(g, start, end, i, getNumStreetSegments());

            } else if (!pinkify) {

                draw_street_name(g, start, end, i, getNumStreetSegments());
            }

        }
    }

    g->set_line_width(4);
    for (size_t i=0; i<getNumStreetSegments(); ++i) {
        LatLon from = getIntersectionPosition(getStreetSegmentInfo(i).from);
        LatLon to = getIntersectionPosition(getStreetSegmentInfo(i).to);
        ezgl::point2d start = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
        ezgl::point2d end = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};
        float street_seg_speed = 3.6 * (getStreetSegmentInfo(i).speedLimit);
        if(!pinkify){
            g->set_color(243,180,46);
        }else{
            g->set_color(108,121,148);
        }

        if (street_seg_speed <90){
            continue;
        }
        // limit the visibility of streets at different scales


        int numCurvesInSeg = getStreetSegmentInfo(i).numCurvePoints;
        // straight line and curves
        if (numCurvesInSeg != 0) {

            ezgl::point2d prev = start;
            for (int j = 0; j < numCurvesInSeg; ++j) {
                LatLon point = getStreetSegmentCurvePoint(j, i);
                ezgl::point2d next = {x_from_lon(point.longitude()), y_from_lat(point.latitude())};
                g->draw_line(prev, next);
                prev = next;
            }
            g->draw_line(prev, end);
        } else {
            g->draw_line(start, end);
        }

        if (scale  > 2.5 && pinkify) {
            g->set_color(ezgl::WHITE);
            draw_street_name(g, start, end, i, getNumStreetSegments());
        } else if (scale > 2.5 && !pinkify) {
            g->set_color(ezgl::BLACK);
            draw_street_name(g, start, end, i, getNumStreetSegments());
        }

    }


    for(int i=0; i<getNumStreetSegments(); ++i){


        if(segment_highlighted[i]){
            g->set_color(ezgl::YELLOW);

            draw_seg_using_seg_id(i, g);
        }
    }

}


// draw all subway routes on map
// initlialize subway and railway routes
void draw_subway_routes(ezgl::renderer* g){
    if (subway_show){
        for (int i=0; i<subway_nodes.size(); ++i){
            LatLon from = subway_nodes[i].first;
            LatLon to = subway_nodes[i].second;
            ezgl::point2d from_from = {x_from_lon(from.longitude()), y_from_lat(from.latitude())};
            ezgl::point2d to_to = {x_from_lon(to.longitude()), y_from_lat(to.latitude())};
            g->set_color(231,126,45);
            g->set_line_width(2);

            g->draw_line(from_from, to_to);


        }
    }

}



// draw main canvas of world on map
void draw_main_canvas_xy_fixed_world (ezgl:: renderer *g) {
    // draw canvas
    if(pinkify){
        ezgl::rectangle visible_world = g->get_visible_world();
        g->set_color(36, 39, 43); // black: DARK MODE
        g->fill_rectangle(visible_world);
    }
    if(!pinkify){
        ezgl::rectangle visible_world = g->get_visible_world();
        g->set_color(215, 215, 215);
        g->fill_rectangle(visible_world);
    }

    g->set_line_width(10);
    g->set_color(255, 248, 196);
    g->draw_rectangle({0,0},{1000,1000});

    //draw all features on map
    //draw all features on map in order
    double map_scale = distance_scaling_fct(g);

    draw_parks(g);
    if(map_scale > 0.195) {
        draw_buildings(g);
    }
    draw_lakes(g);
    draw_rivers(g);
    draw_islands(g);
    draw_beaches(g);


    if(map_scale > 0.025) {
        draw_greenspaces(g);
    }
    if(map_scale > 0.025) {
        draw_golfcourses(g);
    }
    if(map_scale > 0.025) {
        draw_glaciers(g);
    }
    if(map_scale > 0.025) {
        draw_streams(g);
    }

    draw_street_segments(g, map_scale);
    draw_subway_routes(g);
    // subway_station
    if(map_scale > 0.145) {
        draw_subway_station(g);
    }
    if(map_scale > 0.685) {
        draw_toilet(g);
    }
    if(map_scale > 3) {
        draw_toilet_label(g);
    }
    if(map_scale > 0.685) {
        draw_toilets_wheelchair(g);
    }
    if(map_scale > 3) {
        draw_toilets_wheelchair_label(g);
    }
    initial_intersections();
    draw_intersections(g);

    if(!pinkify){
        g->set_color(ezgl::BLACK);
    }
    if(pinkify){
        g->set_color(ezgl::WHITE);
    }

}

// gtk mouse click
void act_on_mouse_click(ezgl::application* app, GdkEventButton* /*event*/, double x, double y) {
    std::cout << "Mouse clicked at (" << x << "," << y << ")\n";
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
    int inter_id = findClosestIntersection(pos);
    std::cout << "Closest Intersection: " << intersections[inter_id].name << "\n";

// pop up the basic information of the intersection
    if (!intersections[inter_id].highlight && !find_path_pressed && (intersections[inter_id].name != "<unknown>")){
        std::ostringstream message_stream;
        message_stream.precision(100);
        message_stream << std::fixed;
        message_stream << "The name of the intersection is : " << intersections[inter_id].name << "\n";
        message_stream << "Mouse clicked at (" << std::to_string(x) << ", " << std::to_string(y) << ")" << "\n";
        std::string combinedMessage = message_stream.str();

        const char* title = "Information";
        const char* message = combinedMessage.c_str();
        app->create_popup_message(title, message);
    }


        intersections[inter_id].highlight = !intersections[inter_id].highlight;
        highlighted.push_back(inter_id);


    if ((highlighted.size() == 2)&&(find_path_pressed)){
        std::pair<IntersectionIdx, IntersectionIdx> start_end;
        start_end.first = highlighted[0];
        start_end.second = highlighted[1];


        std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(15,start_end);

        find_direction(path);
        find_total_time(path);

        std::cout<< "Path size is:"<<path.size() << std::endl;

        for(int i=0; i<popped.size();++i){
            explore_highlighted[popped[i]] = true;

        }


        for(int i=0; i<path.size(); ++i){

            segment_highlighted[path[i]] = true;
        }
    }

    app->refresh_drawing();
}

// gtk pinkify with text status display
void toogle_pink(GtkWidget* /*widget*/, ezgl::application* application ){
    pinkify = ! pinkify;
    GObject* gtk_object = application -> get_object ("TextEntry1");

    GtkEntry* gtk_entry = GTK_ENTRY(gtk_object);
    const gchar* text = gtk_entry_get_text(gtk_entry);
    application -> update_message(text);
    application->refresh_drawing();
}

//Take user map input and give back to main


// take map change input
void take_map_input(GtkWidget* /*widget*/, ezgl::application* application){

    GObject* city_name_obj = application -> get_object ("map_address_input1");
    GtkEntry* city_name_ent = GTK_ENTRY(city_name_obj);
    const gchar* user_city_input = gtk_entry_get_text(city_name_ent);

    //std::cout<<"Map input is: " <<user_city_input <<std::endl;
    bool input_valid = false;

    for(int i=0; i<maps.size(); ++i){
        if (user_city_input==maps[i]){
            input_valid = true;
            break;
        }
    }

    // warning messages
    if (!input_valid){
        // pop up the basic information of the intersection

        std::string combinedMessage = "Please enter a different map name!";

        const char* title = "Warning";
        const char* message = combinedMessage.c_str();
        application->create_popup_message(title, message);
        application->refresh_drawing();

        return;
    }


    std::string map_path = "/cad2/ece297s/public/maps/";
    map_path.append(user_city_input);
    map_path.append(".streets.bin");

    closeMap();

    loadMap(map_path);
    load_max_min_lat_lon();
    ezgl::rectangle initial_world({x_from_lon(min_lon), y_from_lat(min_lat)},
                                  {x_from_lon(max_lon), y_from_lat(max_lat)});

    application->change_canvas_world_coordinates("MainCanvas",initial_world);
    ezgl::renderer *rend = application->get_renderer();
    rend->set_visible_world(initial_world); //update the renderer to have the coords of the new world location


    load_subway_png();
    load_toilet_png();
    load_toilet_wheelchair_png();
    load_location_png();

    draw_main_canvas_xy_fixed_world(rend);
    application->refresh_drawing();
}

//find intersection through two search bar, one is entering the first street name,
//another is the second one, both do not need to enter the full name
//then return the intersection
bool street_contain_special_char (std::string street) {
    std::string special_chars = "~`!@#$%^*()-={[}]/?;:><,.";
    for (int i = 0; i <street.size(); ++i) {
        size_t special_index = special_chars.find(street[i]);
        if (special_index < special_chars.size()) {
            return true;
        }
    }
    return false;
}

bool isInt(const std::string& str) {

    size_t startPos = 0;

    // Check each character in the string
    for (size_t i = startPos; i < str.length(); ++i) {
        if (!isdigit(str[i])) // If any non-digit character found
            return false;
    }

    return true; // All characters are digits
}


void find_intersections(GtkWidget* /*widget*/, ezgl::application* application){

    GObject* street1_obj = application -> get_object ("TextEntry1");
    GtkEntry* input_ent = GTK_ENTRY(street1_obj);
    const gchar* input_text = gtk_entry_get_text(input_ent);
    std::string input;
    input.append(input_text);

    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    input.erase(std::remove_if(input.begin(),input.end(),
                                       ::isspace), input.end());

    //Error message check
    //The entry can not be empty
    if (input.empty()) {
        // pop up the basic information of the intersection

        std::string combinedMessage = "Street names can not be empty!";

        const char *title = "Warning";
        const char *message = combinedMessage.c_str();
        application->create_popup_message(title, message);
        application->refresh_drawing();

        return;
    }

    //if is int, highlight the spot directly
    //This section is only for program testings
    if(isInt(input)){
        int index = std::stoi(input);
        if (index < getNumIntersections()){
            intersections[index].highlight= true;
        }
    }

    //The input can not contain the special character
    if (street_contain_special_char(input)) {
        // pop up the basic information of the intersection

        std::string combinedMessage = "Street names can not have special characters!";

        const char* title = "Warning";
        const char* message = combinedMessage.c_str();
        application->create_popup_message(title, message);
        application->refresh_drawing();

        return;
    }

    //The input should be "street1 & street2"
    size_t pos = input.find("&");

    if(pos == -1){
        std::string combinedMessage = "Please use formal Street 1 & Street 2";

        const char *title = "Warning";
        const char *message = combinedMessage.c_str();
        application->create_popup_message(title, message);
        application->refresh_drawing();

        return;
    }

    //get the street1 and street2 from the user input
    std::string street1 = input;
    std::string street2 = input;

    street1.replace(pos,input.size(),"");
    street2.replace(0, pos+1,"");

    // warning messages
    std::vector<StreetIdx> street_ids_from_partial_street_input1 = findStreetIdsFromPartialStreetName(street1);
    std::vector<StreetIdx> street_ids_from_partial_street_input2 = findStreetIdsFromPartialStreetName(street2);

    if((street_ids_from_partial_street_input1[0] == -1) || (street_ids_from_partial_street_input2[0] == -1)){

        std::string combinedMessage = "Unknown Street!";

        const char* title = "Warning";
        const char* message = combinedMessage.c_str();
        application->create_popup_message(title, message);
        application->refresh_drawing();

        return;
    }
    StreetIdx street1_id;
    StreetIdx street2_id;
    street1_id = street_ids_from_partial_street_input1[0];
    street2_id = street_ids_from_partial_street_input2[0];

    if (street1_id == street2_id) {
        // pop up the basic information of the intersection

        std::string combinedMessage = "Street names can not be the same!";

        const char* title = "Warning";
        const char* message = combinedMessage.c_str();
        application->create_popup_message(title, message);
        application->refresh_drawing();

        return;
    }

    //Find the path from two intersections entered
    std::vector<IntersectionIdx>  intersections_of_two_streets = findIntersectionsOfTwoStreets({street1_id,street2_id});




    for (int i=0; i<intersections_of_two_streets.size(); ++i){
            intersections[intersections_of_two_streets[i]].highlight= true;
            highlighted.push_back(intersections_of_two_streets[i]);
    }
    //if there is two intersections and the find_path button is pressed, find the path
    if ((highlighted.size() == 2)&&(find_path_pressed)){
        std::pair<IntersectionIdx, IntersectionIdx> start_end;
        start_end.first = highlighted[0];
        start_end.second = highlighted[1];

        std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(0,start_end);

        find_direction(path);
        find_total_time(path);

        for(int i=0; i<path.size(); ++i){
            segment_highlighted[path[i]] = true;
        }
    }

    application->refresh_drawing();
}

// initialize dark mode switch
void initial_setup(ezgl::application* application, bool /*new_window*/){
    application->create_button("Pinkify", 6, toogle_pink);
}

// GTK_SWITCH to pinkify canvas
void textEntry_enter (GtkEntry* textEntryWidget, ezgl::application* application) {
    const gchar* text = gtk_entry_get_text (textEntryWidget);
    application->update_message(text);
}

// check if dark mode is selected
gboolean pinkify_switch (GtkSwitch* /*pinkify_switch*/, gboolean switch_state, ezgl::application* application) {
    pinkify = switch_state;
    application -> refresh_drawing();
    return false;
}

// check if subway switch is selected
gboolean show_subways (GtkSwitch* /*subways_switch*/, gboolean switch_state, ezgl::application* application) {
    subway_show = switch_state;
    application -> refresh_drawing();
    return false;
}

// initialize labels and switchs
void initial_setup_pinkify_switch_with_text (ezgl::application* application, bool /*new window*/) {
    GObject* pink_switch = application  -> get_object ("PinkSwitch");
    GObject* subway_switch = application  -> get_object ("subway_switch");
    g_signal_connect (pink_switch, "state_set", G_CALLBACK(pinkify_switch), application);
    g_signal_connect (pink_switch, "activate", G_CALLBACK(textEntry_enter), application);
    g_signal_connect (subway_switch, "state_set", G_CALLBACK(show_subways), application);
    application->create_button("Find Intersections", 8, find_intersections);
    application->create_button("Load Map", 10, take_map_input);
    application->create_button("Clear Highlight", 15, clear_highlight);
    application->create_button("Find Path", 9, find_path);
    application->create_button("Help? Click me :)", 18, help_information);
    application->create_button("Direction", 17, display_direction);
}
void find_total_time(std::vector<StreetSegmentIdx> path){
    for(int i = 0; i<path.size(); ++i){
        double time = findStreetSegmentLength(i)/getStreetSegmentInfo(i).speedLimit;
        total_time += time;
    }
    //total time need to be minutes
    if ((0 < total_time) && ( 60 >= total_time)){
        total_time = 1;
    }
    if (60 < total_time){
        total_time = total_time/60;
        if (total_time%60 >=30){
            total_time ++;
        }
    }
}
//find the direction from the path
void find_direction(std::vector<StreetSegmentIdx> path){
    StreetIdx curr_streetID;
    StreetIdx pre_streetID;
    StreetIdx next_streetID;
    double total_length = 0.0;
    int turn = 0;

    for (int i = 0; i < path.size(); ++i){
        //get the length and the streetID for each segment
        double length = findStreetSegmentLength(path[i]);
        curr_streetID = getStreetSegmentInfo(path[i]).streetID;
        if (i == path.size() -1 ){
            next_streetID = curr_streetID;
        }else {
            next_streetID = getStreetSegmentInfo(path[i + 1]).streetID;
        }
        if (i == 0){
            pre_streetID = getStreetSegmentInfo(path[i]).streetID;
        }
        //add length for each segment
        total_length += length;

        //cases for get the turn
        //stored the streetID and length
        //turn increment
        if ((curr_streetID != next_streetID) && (i == 0)){
            Path_street_length.resize(turn);
            Path_street_length.push_back(std::make_pair(curr_streetID, total_length));

            //to determine turn right or left
            turn_to.resize(turn);
            turn_to.push_back(find_turn_to(path[i], path[i+1]));

            total_length = 0;
            turn ++;
        }

        else if (((pre_streetID != curr_streetID) && (curr_streetID != next_streetID)) &&((0<i)&&(i<path.size()-1))){
            Path_street_length.resize(turn);
            Path_street_length.push_back(std::make_pair(curr_streetID, total_length));

            turn_to.resize(turn);
            turn_to.push_back(find_turn_to(path[i], path[i+1]));

            total_length = 0;
            turn ++;
        }

        else if (((pre_streetID == curr_streetID) && (curr_streetID != next_streetID)) &&((0<i)&&(i<path.size()-1))){
            Path_street_length.resize(turn);
            Path_street_length.push_back(std::make_pair(curr_streetID, total_length));

            turn_to.resize(turn);
            turn_to.push_back(find_turn_to(path[i], path[i+1]));

            total_length = 0;
            turn ++;
        }

        else if ((pre_streetID != curr_streetID) && (i == path.size()-1)){
            Path_street_length.resize(turn);
            Path_street_length.push_back(std::make_pair(curr_streetID, total_length));

            total_length = 0;
            turn++;

        }

        else if ((pre_streetID == curr_streetID) && (i == path.size()-1)){
            Path_street_length.resize(turn);
            Path_street_length.push_back(std::make_pair(curr_streetID, total_length));

            total_length = 0;
            turn++;

        }

        pre_streetID = curr_streetID;
    }

    display_road_length();

}

// helper function for find the turn to right or left
// from "first" street segment turn to "second" street segment
// use the cross product
std::string find_turn_to(StreetSegmentIdx first, StreetSegmentIdx second) {
    IntersectionIdx p1_x = 0;
    IntersectionIdx p2_x = 0;
    IntersectionIdx p3_x = 0;
    IntersectionIdx p1_y = 0;
    IntersectionIdx p2_y = 0;
    IntersectionIdx p3_y = 0;
    std::string turn;

    IntersectionIdx point1 = getStreetSegmentInfo(first).from;
    IntersectionIdx point2 = getStreetSegmentInfo(first).to;
    IntersectionIdx point3 = getStreetSegmentInfo(second).from;
    IntersectionIdx point4 = getStreetSegmentInfo(second).to;

    if (point1 == point3){
        p1_x = x_from_lon(getIntersectionPosition(point2).longitude());
        p2_x = x_from_lon(getIntersectionPosition(point1).longitude());
        p3_x = x_from_lon(getIntersectionPosition(point4).longitude());
        p1_y = y_from_lat(getIntersectionPosition(point2).latitude());
        p2_y = y_from_lat(getIntersectionPosition(point1).latitude());
        p3_y = y_from_lat(getIntersectionPosition(point4).latitude());
    }

    if (point1 == point4){
        p1_x = x_from_lon(getIntersectionPosition(point2).longitude());
        p2_x = x_from_lon(getIntersectionPosition(point1).longitude());
        p3_x = x_from_lon(getIntersectionPosition(point3).longitude());
        p1_y = y_from_lat(getIntersectionPosition(point2).latitude());
        p2_y = y_from_lat(getIntersectionPosition(point1).latitude());
        p3_y = y_from_lat(getIntersectionPosition(point3).latitude());
    }

    if (point2 == point3){
        p1_x = x_from_lon(getIntersectionPosition(point1).longitude());
        p2_x = x_from_lon(getIntersectionPosition(point2).longitude());
        p3_x = x_from_lon(getIntersectionPosition(point4).longitude());
        p1_y = y_from_lat(getIntersectionPosition(point1).latitude());
        p2_y = y_from_lat(getIntersectionPosition(point2).latitude());
        p3_y = y_from_lat(getIntersectionPosition(point4).latitude());
    }

    if (point2 == point4){
        p1_x = x_from_lon(getIntersectionPosition(point1).longitude());
        p2_x = x_from_lon(getIntersectionPosition(point2).longitude());
        p3_x = x_from_lon(getIntersectionPosition(point3).longitude());
        p1_y = y_from_lat(getIntersectionPosition(point1).latitude());
        p2_y = y_from_lat(getIntersectionPosition(point2).latitude());
        p3_y = y_from_lat(getIntersectionPosition(point3).latitude());
    }

    double delta_x1 = p2_x - p1_x;
    double delta_y1 = p2_y - p1_y;
    double delta_x2 = p3_x - p2_x;
    double delta_y2 = p3_y - p2_y;

    //turn right -> (cross product < 0)
    //turn left -> (cross product > 0)
    //go straight -> (cross product = 0)
    double cross_product = delta_x1 * delta_y2 - delta_y1 * delta_x2;
    if (cross_product > 0) {
        turn = "left";
    }
    if (cross_product < 0) {
        turn = "right";
    }
    return turn;
}

int roundToNearestHundred(double num) {
    if (num <= 100) return 100; // Handle edge case where number is less than or equal to 100

    // Round the number to the nearest hundred
    int rounded = static_cast<int>(std::round(num / 100.0) * 100);
    return rounded;
}

void display_road_length(){
    for (int i = 0; i < Path_street_length.size(); i++){
        int length = Path_street_length[i].second;
        total_distance += Path_street_length[i].second;
        if ( length < 1.5 ) {
            length = 1;
        }
        else if (( 1.5 <= length) && (length < 2.5 )) {
            length = 2;
        }
        else if (( 2.5 <= length) &&( length < 3.5)) {
            length = 3;
        }
        else if (( 3.5 <= length) &&( length < 4.5 )) {
            length = 4;
        }
        else if (( 4.5 <= length) &&( length < 7.5 )) {
            length = 5;
        }
        else if (( 7.5 <= length) &&( length < 15 )) {
            length = 10;
        }
        else if (( 15 <= length) &&( length < 35 )) {
            length = 20;
        }
        else if (( 35 <= length) &&( length < 75 )) {
            length = 50;
        }
        else if (length >= 75) {
            length = roundToNearestHundred(length);
        }
        path_length.push_back(length);
//        std::cout<<"pre length: " << Path_street_length[i].second <<std::endl;
//        std::cout<<"length: " << path_length[i] <<std::endl;
    }
    total_distance = roundToNearestHundred(total_distance);
}



void display_direction(GtkWidget* /*widget*/, ezgl::application* application){
    //The Find_path button is pressed and the two intersections is highlighted,
    // display the direction information
    if ((find_path_pressed) && ( highlighted.size()>1)) {
        std::ostringstream message_stream;
        message_stream << "The total distance is " << total_distance << " meters." << "\n";
        message_stream << "It takes " << total_time <<" minutes to reach the destination." << "\n";
        message_stream << "\n";
        for (int i = 0; i < Path_street_length.size(); ++i) {
            if ( i < Path_street_length.size() - 1) {
                message_stream.precision(1);
                message_stream << std::fixed;
                message_stream << "Go straight down the " << getStreetName(Path_street_length[i].first)
                               << " for " << path_length[i]
                               << " meters and then turn to " << turn_to [i] << "\n";

            }

            if (i == Path_street_length.size() - 1) {
                message_stream << "Go straight down the " << getStreetName(Path_street_length[i].first)
                               << " for " << path_length[i]
                        << " meters. " << "\n";
                message_stream << "You have reached you destination! " << "\n";

            }
        }


        std::string combinedMessage = message_stream.str();

        const char *title = "Direction";
        const char *message = combinedMessage.c_str();
        application->create_popup_message(title, message);
    }
    //else display no directions
    else{
        std::ostringstream message_stream;
        message_stream.precision(100);
        message_stream << std::fixed;
        message_stream << "Please find the path to get the direction! " << "\n";

        std::string combinedMessage = message_stream.str();

        const char* title = "Direction";
        const char* message = combinedMessage.c_str();
        application->create_popup_message(title, message);
    }

}

//clear all highlights on the map
void clear_highlight(GtkWidget* /*widget*/, ezgl::application* application){
    for (int i=0; i<getNumIntersections(); ++i){
        intersections[i].highlight = false;
    }

    for (int i=0; i<getNumStreetSegments(); ++i){
        segment_highlighted[i]=false;
        explore_highlighted[i]=false;
    }

    std::vector<IntersectionIdx> ().swap(highlighted);
    std::vector<StreetSegmentIdx>().swap(popped);

    //clear all visited and reaching edge of Nodes
    for (int i=0; i<Nodes.size(); ++i){
        Nodes[i].visited = false;
        Nodes[i].best_route_time = 99999;
    }

    application -> refresh_drawing();
}

//display the path on the map
void find_path (GtkWidget* /*widget*/, ezgl::application* application){
    clear_highlight(nullptr, application);
    find_path_pressed = true;

}

//Help
void help_information(GtkWidget* /*widget*/, ezgl::application* application){
    std::ostringstream message_stream;
    message_stream.precision(100);
    message_stream << std::fixed;
    message_stream << "Search for intersections: " << "\n";
    message_stream << "Intersection: Enter the first street name, type '&', and enter the second street name. eg. yonge&college" << "\n";
    message_stream << "Click the 'Find Intersection' button to find" << "\n";
    message_stream << "\n";
    message_stream << "Find path: " << "\n";
    message_stream << "Click 'Find path' firstly, and click two intersections on the map to find path. " << "\n";
    message_stream << "Or you can enter two intersections to find path. " << "\n";
    message_stream << "\n";
    message_stream << "Load map: " << "\n";
    message_stream << "City name: Enter the city name, type '_', and enter the country name. eg. toronto_canada " << "\n";
    message_stream << "Click the 'Load Map' button to load. " << "\n";
    message_stream << "\n";
    message_stream << "Find Direction: " << "\n";
    message_stream << "After you find the path, you can click the direction button to get the navigation. " << "\n";
    std::string combinedMessage = message_stream.str();

    const char* title = "Help";
    const char* message = combinedMessage.c_str();
    application->create_popup_message(title, message);
}

//draw the map here
void draw_map_intersections_xy_fixed_world() {
    ezgl::application::settings settings;

    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier="MainWindow";
    settings.canvas_identifier="MainCanvas";
    // Create our EZGL application
    ezgl::application application(settings);
    ezgl::rectangle initial_world({x_from_lon(min_lon), y_from_lat(min_lat)},
                                  {x_from_lon(max_lon), y_from_lat(max_lat)});
    application.add_canvas("MainCanvas", draw_main_canvas_xy_fixed_world, initial_world);
    // Run the application until the user quits



    application.run(initial_setup_pinkify_switch_with_text, act_on_mouse_click, nullptr, nullptr);
    // run initial setup using pinkify switch callback
}
