#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

//enum {END, NUMERIC, ALNUM, STRUCTURED, BYTE, FNC1, ZER ,ECI, KANJI};
//enum Type { NUMERIC,           ALPHANUM,          BYTE,             KENJI};
enum Ecc {L,M,Q,H};
enum Type {NUMERIC, ALPHANUM,BYTE, KANJI};
enum Mtype{MODE, MODSIZE};
const int Mdbase[4][2] = {{1, 4},{2, 6},{4, 8},{8, 16}};
//         0 -> 1             1 -> 2              2 -> 4           3 -> 8
//        NUMERIC          alphanumeric            BYTE             kanji
//    L   M   Q   H       L   M   Q   H       L   M   Q   H     L   M   Q   H
const int base[40][4][4] =
{{{                  },{              	 },{           	   },{               }},
{{  41,  34,  27,  17},{ 25,  20, 16,  10},{ 17, 14, 11,  7},{ 10,  8,  7,  4}},
{{  77,  63,  48,  34},{ 47,  38, 29,  20},{ 32, 26, 20, 14},{ 20, 16, 12,  8}},
{{ 127, 101,  77,  58},{ 77,  61, 47,  35},{ 53, 42, 32, 24},{ 32, 26, 20, 15}},
{{ 187, 149, 111,  82},{114,  90, 67,  50},{ 78, 62, 46, 34},{ 48, 38, 28, 21}},
{{ 255, 202, 144, 106},{154, 122, 87,  64},{106, 84, 60, 44},{ 65, 52, 37, 27}},
{{ 322, 255, 178, 139},{195, 154, 108, 84},{134,106, 74, 58},{ 82, 65, 45, 36}},
{{ 370, 293, 207, 154},{224, 178, 125, 93},{154,122, 86, 64},{ 95, 75, 53, 39}},
{{ 461, 365, 259, 202},{279,221, 157, 122},{192,152,108, 84},{118, 93, 66, 52}},
{{ 552, 432, 312, 235},{335,262, 189, 143},{230,180,130, 98},{141,111, 80, 60}},
{{ 652, 513, 364, 288},{395,311, 221, 174},{271,213,151,119},{167,131, 93, 74}},
{{ 772, 604, 427, 331},{468,366, 259, 200},{321,251,177,137},{198,155,109, 85}},
{{ 883, 691, 489, 374},{535,419, 296, 227},{367,287,203,155},{226,177,125, 96}},
{{1022, 796, 580, 427},{619,483, 352, 259},{425,331,241,177},{262,204,149,109}},
{{1101, 871, 621, 468},{667,528, 376, 283},{458,362,258,194},{282,223,159,120}},
{{1250, 991, 703, 530},{758,600, 426, 321},{520,412,292,220},{320,254,180,136}},
{{1408,1082, 775, 602},{854,656, 470, 365},{586,450,322,250},{361,277,198,154}},
{{1548,1212, 876, 674},{938,734, 531, 408},{644,504,364,280},{397,310,224,173}},
{{1725,1346, 948, 746},{1046,816,574, 452},{718,560,394,310},{442,345,243,191}},
{{1903,1500,1063, 813},{1153,909,644, 493},{792,624,442,338},{488,384,272,208}},
{{2061,1600,1159, 919},{1249,970,702, 557},{858,666,482,382},{528,410,297,235}},
{{2232,1708,1224, 969},{1352,1035,742,587},{929,711,509,403},{572,438,314,248}},
{{2409,1872,1358,1056},{1460,1134,823,640},{1003,779,565,439},{618,480,348,270}}};
const int err_blocks [4][41] = {
    //  Version: (note that index 0 is for padding, and is set to an illegal value)
    //  1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
    {0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
    {0, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
    {0, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
    {0, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
};
//const int version[5] = {0, 21, 25, 29, 33};
const int timing = 6, infos = 8, pad = 9;
vector<string> padbyte = {"11101100","00010001"};

class QRcode {
    public :
    size_t version, ecc, mask, mode;
    int *grid;
    size_t msize;
    string bits;
};

void dictionary () {

  /*
    version 1 :      2:
        size    21  25

    version :
        version[new->version] // matrix size

    ecc level :
        {0,0} H; // 0
        {0,1} Q; // 1
        {1,0} M; // 2
        {1,1} L; // 3

    mask :
        {0,0,0} (i * j) % 2 + (i * j) % 3  // 0
        {0,0,1} (i / 2 + j / 3) % 2        // 1
        {0,1,0} ((i * j) % 3 + i + j) % 2  // 2
        {0,1,1} ((i + j) % 3 + i * j) % 2  // 3
        {1,0,0} i % 2                      // 4
        {1,0,1} (i + j % 2)                // 5
        {1,1,0} (i + j) % 3                // 6
        {1,1,1} j % 3                      // 7

    format err corr :

    mode :
        {0,0,0,0} end msg     // 0
        {0,0,0,1} numeric;    // 1
        {0,0,1,0} alnum;      // 2
        {0,0,1,1} structured  // 3
        {0,1,0,0} byte        // 4
        {0,1,0,1} FNC1 fst    // 5
        {0,1,1,1} ECI         // 7
        {1,0,0,0} kanji       // 8
        {1,0,0,1} FNC1 scd    // 9

    */

}

void display (QRcode &qr) {
    const int size = (17 + qr.version * 4);

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x)
            qr.grid[y * size + x] ? cout << "# " : cout << "  ";

        cout << endl;
    }

}

bool if_mask (int mask, int i, int j) {

  bool tag = true;

  switch (mask) {
      case 0 : tag = (i * j) % 2 + (i * j) % 3; break;
      case 1 : tag = (i / 2 + j / 3) % 2; break;
      case 2 : tag = ((i * j) % 3 + i + j) % 2; break;
      case 3 : tag = ((i + j) % 3 + i * j) % 2; break;
      case 4 : tag = i % 2; break;
      case 5 : tag = (i + j) % 2; break;
      case 6 : tag = (i + j) % 3; break;
      case 7 : tag = j % 3; break;
      default : break;
  }

  return tag == 0;
}
bool is_grid (QRcode &qr, int x, int y) {
    const int lim = (17 + qr.version * 4) - pad;
    const int dark = (4 * qr.version + 10);
    if (x < 0 || y < 0) return false;

    if (qr.version > 1)
        if ((x >= (dark - 2) && x <= (dark + 2)) && (y >= (dark - 2) && y <= (dark + 2))) return false;

    if (x == 8 && y == (4 * qr.version + 10)) return false;// dark module
    //if (*(ptr + (dark * size + 8)) = 1; // dark module)
    if (x == timing || y == timing) return false; // timming pattern
    if (x > lim && y < pad) return false; // upper right corner
    if (x < pad && (y < pad || y > lim)) return false;

    return true;
}
///////////////////////// decoding ///////////////////////////////
string scan (QRcode &qr) {

    if (!qr.version) throw exception ();

    const int size = (17 + qr.version * 4);
    int i, startx = size, endx = startx - 1, x = startx, y;
    bool up = true, bit;
    string bits;

    while (x >= 0) {
        for (i = 0; i < size; i++) {
            y = up == true ? (size - 1) - i : i;

            for (x = startx; x-->= endx;)

                if (is_grid (qr, x, y)) {
                    bit = qr.grid[y * size + x];
                    bits += to_string (if_mask (qr.mask, x, y) ? !bit : bit);
                }
        }

      up ^= 1;
      startx = endx - 1, endx = startx - 1;
    }
    return bits;
}
QRcode informations (int qr[21][21]) {

    QRcode next;
    string format;

    next.grid = *qr;
    next.version = 1;

    const int size = 17 + next.version * 4;

    for (int i = 0; i < size; ++i)
        format += to_string (qr[infos][i]);

    next.ecc = bitset<2>(format.substr (0, 2)).to_ulong();
    next.mask = bitset<3>(format.substr (2, 3)).to_ulong();

    next.bits = scan (next);

    next.mode = bitset<4>(next.bits.substr(0, 4)).to_ulong();
    next.msize = bitset<8>(next.bits.substr(4, 8)).to_ulong();

    return next;
}
string scanner (int qrcode[21][21]) {

    QRcode qr = informations (qrcode);
    const int end = qr.msize * 8 + 12;

    string byte, os;

    for (int i = 12; i < end; i += 8) {
        byte = qr.bits.substr(i, 8);
        os += bitset<8>(byte).to_ulong();
    }

    return os;
}
///////////////////////// drawing ///////////////////////////////
void mk_pattern (QRcode qr, int *ptr) {
    const int size = (17 + qr.version * 4);

    for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 7; ++x) {
            if (x == 0 || x == 6 || y == 0 || y == 6)
                ptr[y * size + x] = 1;
            else if ((x > 1 && x < 5) && (y > 1 && y < 5))
                ptr[y * size + x] = 1;

        }
    }
}
void mk_align (QRcode qr, int *ptr) {
    const int size = (17 + qr.version * 4);

    for (int y = -2; y < 3; ++y) {
        for (int x = -2; x < 3; ++x) {
            if (abs (x) == 2 || abs (y) == 2)
                ptr[y * size + x] = 1;
            else if (x == 0 && y == 0)
                ptr[y * size + x] = 1;
        }
    }
}
void draw (QRcode &qr) {

    if (!qr.version) throw exception ();

    const int size = (17 + qr.version * 4);
    const int lim = (17 + qr.version * 4) - pad;

    qr.grid = new int[size * size]();

    int i, *ptr = &qr.grid[0];
    int dark = (4 * qr.version + 10);

    mk_pattern(qr, ptr);                        // position pattern
    mk_pattern(qr, (ptr + lim + 2));
    mk_pattern(qr, (ptr + (lim + 2) * size));

    if (qr.version > 1)
        mk_align (qr, (ptr + dark * size + dark));  // align pattern

    for (i = pad - 1; i < lim + 1; i += 2) {    // timming pattern
        qr.grid[i * size + 6] = 1;
        qr.grid[6 * size + i] = 1;
    }

    *(ptr + (dark * size + 8)) = 1;             // dark module
}
///////////////////////// encoding ///////////////////////////////
void encode_byte (QRcode &qr, string &str) {

  const int end = base[qr.version][qr.mode][qr.ecc] - qr.msize;

  qr.bits += bitset<4>(qr.mode).to_string();
  qr.bits += bitset<8>(str.size()).to_string();
  //string msg;
  for (auto &it : str)
      qr.bits += bitset<8>(it).to_string();

      qr.bits += "0000";

  for (int i = 0; i < end; ++i)
        qr.bits += padbyte[i % 2];
}

int get_mode (string src) {

    enum Type mode = NUMERIC;

    for (auto &it : src) {
        if (isdigit (it)) mode = max (mode, NUMERIC);//mode = max (mode , 1); // numeric
        if (isupper (it)) mode = max (mode, ALPHANUM); // alphanumeric
        if (islower (it)) mode = max (mode, BYTE); // byte
        // if (iseci (it)) mode = max (mode, ECI); // ECI
        // if (iskanji (it)) mode = max (mode, KANJI); // kanji
    }

    return mode;
}
QRcode create (string iss, int err) {
    QRcode next;
    int size = iss.size(), v = 0, num, mode = get_mode (iss);

    next.ecc = err;
    next.mode = mode;
    next.msize = size * Mdbase[mode][MODSIZE] / 8;

    while (true) {
        num = base[v][mode][next.ecc];
        if (num >= size) break;
        else v++;
    }

    next.version = v;

    return next;
}
//////////////////////////////////////////////////////////////////
void apply_bits (QRcode &qr) {

    const int size = 17 + qr.version * 4; //(17 + qr.version * 4);
    const int msize = qr.bits.size(), dark = (4 * qr.version + 10);
    int i, startx = size, endx = startx - 1, x = startx, y, n = 0;
    bool up = true, bit;
    string bits = qr.bits;

    while (x >= 0) {
        for (i = 0; i < size; i++) {
            y = up == true ? (size - 1) - i : i;
            for (x = startx; x-->= endx;) {
                bit = qr.grid[y * size + x];

                if (is_grid (qr, x, y)) {
                    //bit = (n <= msize) ? (bits[n++] - '0') : 0;
                    if (n <= msize)
                        bit = bits[n++] - '0';
                }

                qr.grid[y * size + x] = bit;
            }
        }

      up ^= 1;
      startx = endx - 1, endx = startx - 1;
    }
}
vector<int> mk_mask (QRcode &qr, int mask) {

  const int size = (17 + qr.version * 4);
  int i, startx = size, endx = startx - 1, x = startx, y, pos;
  bool up = true, bit;

  vector<int> next (size * size);

  while (x >= 0) {
    for (i = 0; i < size; i++) {
      y = up == true ? (size - 1) - i : i;

      for (x = startx; x-->= endx;) {

          bit = qr.grid[y * size + x];
          if (is_grid (qr, x, y)) {
              if (if_mask (mask, x, y))
                bit ^= 1;

          }
          next[y * size + x] = bit;
        }
    }

    up ^= 1;
    startx = endx - 1, endx = startx - 1;
  }

  return next;
}

int main () {

  int value[21][21]= {{ 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1 },
                      { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
                      { 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1 },
                      { 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1 },
                      { 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1 },
                      { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1 },
                      { 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1 },
                      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
                      { 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 },
                      { 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1 },
                      { 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1 },
                      { 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1 },
                      { 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1 },
                      { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0 },
                      { 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1 },
                      { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
                      { 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1 },
                      { 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 },
                      { 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1 },
                      { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 },
                      { 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1 }};

    QRcode qr = informations (value);
    string str = "this is a only a test";
    //str = "warrior";
    QRcode nxt = create (str, L);

    draw (nxt);
    encode_byte (nxt, str);
    apply_bits(nxt);

    display (nxt);
    int codeblocks = base[nxt.version][nxt.mode][nxt.ecc] + 2;
    int n_blocks = err_blocks[nxt.ecc][nxt.version];
    //for (int i = 0; i < 10; i++)

    /*
    const int size = 17 + nxt.version * 4;

    vector<int> grid = mk_mask(nxt, 5);

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            grid[y * size + x] ? cout << "# " : cout << "  ";
        }
        cout << endl;
    }
    */

    //display(nxt);

}
