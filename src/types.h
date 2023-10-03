#ifndef TYPES_H
#define TYPES_H

#include <cstdlib>

// Defines an address of a record stored as a block address with an offset.
struct Address
{
  void *blockAddress;
  uint8_t offset;
};

// Defines a single record (read from data file).
struct Record{
    float fg_pct_home;
    float ft_pct_home;
    float fg3_pct_home;
    int game_date;
    unsigned short int recordID;
    uint8_t team_id_home;
    uint8_t pts_home;
    uint8_t ast_home;
    uint8_t reb_home;
    bool home_team_wins;
};

#endif