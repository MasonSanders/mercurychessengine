#ifndef __MOVETYPE_H__
#define __MOVETYPE_H__

enum class MoveType {
    Quiet,
    Capture,
    DoublePawnPush,
    EnPassant,
    KingCastle,
    QueenCastle,
    Promotion,
    PromotionCapture
};

#endif