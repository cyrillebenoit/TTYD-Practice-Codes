#pragma once

#include <cstdint>

#include <ttyd/party.h>

namespace ttyd::mario_party {

extern "C" {

// partyGetTechLv
// partyGetHp
// partyChkJoin
void partyLeft(party::PartyMembers id);
void partyJoin(party::PartyMembers id);
int32_t marioGetExtraPartyId();
int32_t marioGetPartyId();
// marioGetParty
// marioPartyKill
int32_t marioPartyGoodbye();
int32_t marioPartyHello(party::PartyMembers id);
int32_t marioPartyEntry(party::PartyMembers id);
// marioUseParty

}

}