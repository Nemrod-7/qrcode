

namespace MQR {
////////////////////////////// specifications //////////////////////////////
  enum Mode {NUMERIC, ALPHANUM, BYTE, KANJI};
  enum {M1,M2,M3,M4};
  enum {L,M,Q};

  namespace info {
      const std::string level[4] = {"Low", "Medium", "Quartile"};
      const std::string mode[4] = {"NUMERIC", "ALPHANUM", "BYTE", "KANJI"};
  };

  // the format information forMQR QR differs from the standard QR.
  // the first 3 bits identify the version and the error eorrection used.
  // ie : 000 = (M1 L) 011 = (M3, L)
  // there is no logic behind that. Just take the infor from the two tables below.

  const unsigned vrlevel[8] = {M1,M2,M2,M3,M3,M4,M4,M4};
  const unsigned eclevel[8] = { L, L, M, L, M, L, M, Q};

  // the fourth and fifth data bits of the format information contain
  // the data mask pattern reference, only mask 1, 4, 6 and 7 are allowed
  // for the micro qr version.
  const unsigned mqmask[4] = { 1, 4, 6, 7};

  //                                         [         data codewords         ] [            ecc codewords             ]
  // the qr binary data is encoded like this [[[mode][data length][data][0000]] [ecc block 1][ecc block 2][ecc block...]

  // the message received (numeric, alphanumeric, byte or kanji) is encoded and the error correction code chosen determine the size needed and so the size of the grid.
  // this table is used just to determine the version of the micro qr code and nothing else. : capacity[version][mode][ecc] -> maximum of encoded [data] codewords allowed .
  //
  const unsigned capacity[4][4][3] = {
    //  NUMERIC,    ALPHANUM,    BYTE,    KANJI
    //   L  M  Q    L  M  Q    L  M  Q    L  M  Q
      {{ 5, 0, 0},{ 0, 0, 0},{ 0, 0, 0},{ 0, 0, 0}},
      {{10, 8, 0},{ 6, 5, 0},{ 0, 0, 0},{ 0, 0, 0}},
      {{23,18, 0},{14,11, 0},{ 9, 7, 0},{ 6, 4, 0}},
      {{35,30,21},{21,18,13},{15,13, 9},{ 9, 8, 5}},
  };

  // codewords[ecc][version] -> number of [data codewords] for each version :
  const unsigned codewords[3][4] = {
  //  M1, M2, M3, M4,
    {  3,  5, 11, 16 }, // L
    {  0,  4,  9, 14 }, // M
    {  0,  0,  0, 10 }, // Q
  };

  //  ecsize[ecc][version] -> number of [ecc codwords] needed for each block.
  const unsigned ecsize[3][4] = {
  //  M1, M2, M3, M4,
    {  2,  5,  6,  8 }, // L
    {  0,  6,  8, 10 }, // M
    {  0,  0,  0, 14 }, // Q
  };

  // the number of bits necessary to record the size of the txt
  // depends of the version and the mode :
  //   N A B K
  const unsigned int length_bits[4][4] = {
      {3,0,0,0}, // M1
      {4,3,0,0}, // M2
      {5,4,4,3}, // M3
      {6,5,5,4}, // M4
  };

  int get_mode (const std::string &src);
  std::vector<std::vector<int>> make (int version);
  std::vector<std::vector<int>> write (const std::string &txt, int ecc);

  std::string read (const std::vector<std::vector<int>> &qr);
};
