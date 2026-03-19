#include <vector>
#include <map>
#include <cmath>
#include <limits>

class Point {
    public :
        double x, y;

        Point () { x = 0, y = 0;}
        Point (double a) {x = a, y = 0;};
        Point (double a, double b) { x = a, y = b;}
};

static double distance (const Point &a, const Point &b) { return hypot (a.x - b.x, a.y - b.y); }
static bool is_equal (const Point &a, const Point &b) { return a.x == b.x && a.y == b.y;}

class K {
    private :
        static Point nearest_point (const std::vector<Point> &grph, Point p) {
            Point nearest;
            double min = std::numeric_limits<double>::infinity(), dist;

            for (auto &star : grph) {
                dist = distance (p, star);

                if (dist < min) {
                    min = dist;
                    nearest = star;
                }
            }

            return nearest;
        }

    public :
        static std::vector<Point> means (const std::vector<Point> &galaxy, std::vector<Point> ctid) {

            Point near;
            bool done = true;

           while (done) {
                std::vector<Point> tmp = ctid;
                std::map<double, double> npoints, sumX, sumY;
                done = false;

                for (auto &p : galaxy) {
                    near = nearest_point (ctid, p);
                    for (size_t i = 0; i < ctid.size(); ++i)
                        if (is_equal (ctid[i], near)) {
                            npoints[i]++;
                            sumX[i] += p.x;
                            sumY[i] += p.y;
                        }
                }

                for (size_t i = 0; i < ctid.size(); ++i) {
                    ctid[i].x = sumX[i] / npoints[i];
                    ctid[i].y = sumY[i] / npoints[i];
                   if (!is_equal (ctid[i], tmp[i])) done = true;
                }
            }
            return ctid;
        }
};
