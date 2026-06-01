#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_terastal.h"
#include "battle_gimmick.h"
#include "battle_scripts.h"
#include "event_data.h"
#include "item.h"
#include "palette.h"
#include "pokemon.h"
#include "safari_zone.h"
#include "sprite.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/rgb.h"
#include "debug.h"  // v2.0.3 — Sim_GetBattlerTrainerId / Sim_TrainerCanTera / Sim_IsActive

// Sets flags and variables upon a battler's Terastallization.
void ActivateTera(enum BattlerId battler)
{
    // Set appropriate flags.
    SetActiveGimmick(battler, GIMMICK_TERA);
    SetGimmickAsActivated(battler, GIMMICK_TERA);

    // Remove Tera Orb charge.
    if (B_FLAG_TERA_ORB_CHARGED != 0
        && (B_FLAG_TERA_ORB_NO_COST == 0 || !FlagGet(B_FLAG_TERA_ORB_NO_COST))
        && IsOnPlayerSide(battler)
        && !(IsDoubleBattle() && !IsPartnerMonFromSameTrainer(battler)))
    {
        FlagClear(B_FLAG_TERA_ORB_CHARGED);
    }

    // Execute battle script.
    PREPARE_TYPE_BUFFER(gBattleTextBuff1, GetBattlerTeraType(battler));
    if (TryBattleFormChange(gBattlerAttacker, FORM_CHANGE_BATTLE_TERASTALLIZATION, GetBattlerAbility(gBattlerAttacker)))
        BattleScriptPushCursorAndCallback(BattleScript_TeraFormChange);
    else if (gBattleStruct->illusion[gBattlerAttacker].state == ILLUSION_ON
          && DoesSpeciesHaveFormChangeMethod(GetIllusionMonSpecies(gBattlerAttacker), FORM_CHANGE_BATTLE_TERASTALLIZATION))
        BattleScriptPushCursorAndCallback(BattleScript_IllusionOffAndTerastallization);
    else
        BattleScriptPushCursorAndCallback(BattleScript_Terastallization);
}

// Applies palette blend and enables UI indicator after animation has played
void ApplyBattlerVisualsForTeraAnim(enum BattlerId battler)
{
    struct Pokemon *party = GetBattlerParty(battler);
    u32 index = gBattlerPartyIndexes[battler];

    // Show indicator and do palette blend.
    UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], &party[index], HEALTHBOX_ALL);
    BlendPalette(OBJ_PLTT_ID(battler), 16, 8, GetTeraTypeRGB(GetBattlerTeraType(battler)));
    CpuCopy32(gPlttBufferFaded + OBJ_PLTT_ID(battler), gPlttBufferUnfaded + OBJ_PLTT_ID(battler), PLTT_SIZEOF(16));

    // We apply the animation behind a white screen, so restore the blended color here to avoid a pop
    BlendPalette(OBJ_PLTT_ID(battler), 16, 16, RGB_WHITEALPHA);
}

// Returns whether a battler can Terastallize.
bool32 CanTerastallize(enum BattlerId battler)
{
    enum HoldEffect holdEffect = GetBattlerHoldEffectIgnoreNegation(battler);

    // v2.0.1 — Re-enabled. The v0.41.1 early return is removed now that
    // Scarlet/Violet + Legends Z-A trainers have landed. Gating happens in
    // ShouldTrainerBattlerUseGimmick via Sim_TrainerCanTera (past-gen
    // trainers still can't Tera; Gen 9 + custom slots can).
    //
    // v2.0.3 — Defensive engine-level gate. ShouldTrainerBattlerUseGimmick's
    // gate has multiple branches (AI-vs-AI, player, opponent) and the player
    // branch historically returned TRUE unconditionally, so a past-gen mon
    // could slip through if any single path missed the check. Block at the
    // top of CanTerastallize too — belt-and-suspenders. Frontier battles
    // (Sim_IsActive() == FALSE) skip this gate; Sim_TrainerCanTera handles
    // them permissively anyway.
    // Initial v2.0.3 used gIsDebugBattle here, but that flag is only flipped
    // by the legacy DebugAction_Party_BattleSingle path — Sim_SetupMatchRound
    // never sets it, so the check silently passed in every modern sim battle
    // (which is why Z-A trainers were still Tera-ing). Sim_IsActive() reads
    // gSimPilotMode and the B_FLAG_AI_VS_AI_BATTLE flag, both of which
    // Sim_SetupMatchRound actually maintains.
    if (Sim_IsActive() && !Sim_TrainerCanTera(Sim_GetBattlerTrainerId(battler)))
        return FALSE;

    if (gBattleMons[battler].volatiles.transformed && GET_BASE_SPECIES_ID(gBattleMons[battler].species) == SPECIES_TERAPAGOS)
        return FALSE;

    // Prevents Zigzagoon from terastalizing in vanilla.
    if (gBattleTypeFlags & BATTLE_TYPE_FIRST_BATTLE && !IsOnPlayerSide(battler))
        return FALSE;

    // v2.0.3 — Skip the bag/charge gate in sim mode too. The player side in a
    // sim battle is piloting someone else's loaner team, not the user's actual
    // collection — the "you carry a Tera Orb, you charge it by visiting a
    // Pokemon Center" mechanic doesn't apply. And with the vanilla config
    // defaults (B_FLAG_TERA_ORB_CHARGED = 0, B_FLAG_TERA_ORB_NO_COST = 0),
    // FlagGet(0) returns FALSE, so the !FlagGet(B_FLAG_TERA_ORB_CHARGED)
    // branch below was hard-blocking player-side Tera in every pilot-mode
    // battle — that's why piloting an SV trainer didn't show the Tera option.
    //
    // v2.0.3 second pass — also guard each flag check on `flag != 0`. The
    // first pass relied on Sim_IsActive() being TRUE in pilot mode, but the
    // user re-tested and the Tera button is still hidden, suggesting either
    // Sim_IsActive() is FALSE in their setup or the FlagGet(0) misbehavior is
    // firing through another path. Treat `B_FLAG_TERA_ORB_CHARGED == 0` as
    // "charge mechanic disabled" (orb is always considered usable) rather
    // than "permanently uncharged" — the original engine treats it as the
    // latter, which makes the mechanic completely unusable when unconfigured.
    // The Sim_TrainerCanTera gate above still blocks past-gen pilot teams.
    if (TESTING || !IsOnPlayerSide(battler) || Sim_IsActive())
    {
        // Skip all other checks in this block, go to HasTrainerUsedGimmick
    }
    else if (!CheckBagHasItem(ITEM_TERA_ORB, 1))
    {
        return FALSE;
    }
    else if (B_FLAG_TERA_ORB_NO_COST != 0 && FlagGet(B_FLAG_TERA_ORB_NO_COST))
    {
        // Tera Orb is not depleted, go to HasTrainerUsedGimmick
    }
    else if (B_FLAG_TERA_ORB_CHARGED != 0 && !FlagGet(B_FLAG_TERA_ORB_CHARGED))
    {
        return FALSE;
    }

    // Check if Trainer has already Terastallized.
    if (HasTrainerUsedGimmick(battler, GIMMICK_TERA))
        return FALSE;

    // Check if AI battler is intended to Terastallize.
    if (!ShouldTrainerBattlerUseGimmick(battler, GIMMICK_TERA))
        return FALSE;

    // Check if battler has another gimmick active.
    if (GetActiveGimmick(battler) != GIMMICK_NONE)
        return FALSE;

    // Check if battler is holding a Z-Crystal or Mega Stone.
    if (!TESTING && (holdEffect == HOLD_EFFECT_Z_CRYSTAL || holdEffect == HOLD_EFFECT_MEGA_STONE)) // tests make this check already
        return FALSE;

    // Every check passed!
    return TRUE;
}

// Returns a battler's Tera type.
enum Type GetBattlerTeraType(enum BattlerId battler)
{
    return GetMonData(GetBattlerMon(battler), MON_DATA_TERA_TYPE);
}

// Uses up a type's Stellar boost.
void ExpendTypeStellarBoost(enum BattlerId battler, enum Type type)
{
    if (type < 32 && gBattleMons[battler].species != SPECIES_TERAPAGOS_STELLAR) // avoid OOB access
        gBattleStruct->stellarBoostFlags[GetBattlerSide(battler)] |= 1u << type;
}

// Checks whether a type's Stellar boost has been expended.
bool32 IsTypeStellarBoosted(enum BattlerId battler, enum Type type)
{
    if (type < 32) // avoid OOB access
        return !(gBattleStruct->stellarBoostFlags[GetBattlerSide(battler)] & (1u << type));
    else
        return FALSE;
}

// Returns the STAB power multiplier to use when Terastallized.
// Power multipliers from Smogon Research thread.
uq4_12_t GetTeraMultiplier(struct BattleContext *ctx)
{
    enum Type teraType = GetBattlerTeraType(ctx->battlerAtk);

    // Safety check.
    if (GetActiveGimmick(ctx->battlerAtk) != GIMMICK_TERA)
        return UQ_4_12(1.0);

    // Stellar-type checks.
    if (teraType == TYPE_STELLAR)
    {
        bool32 shouldBoost = IsTypeStellarBoosted(ctx->battlerAtk, ctx->moveType);
        if (IS_BATTLER_OF_BASE_TYPE(ctx->battlerAtk, ctx->moveType))
        {
            if (shouldBoost)
                return UQ_4_12(2.0);
            else
                return UQ_4_12(1.5);
        }
        else if (shouldBoost)
            return UQ_4_12(1.2);
        else
            return UQ_4_12(1.0);
    }
    // Base and Tera type.
    if (ctx->moveType == teraType && IS_BATTLER_OF_BASE_TYPE(ctx->battlerAtk, ctx->moveType))
    {
        if (ctx->abilityAtk == ABILITY_ADAPTABILITY)
            return UQ_4_12(2.25);
        else
            return UQ_4_12(2.0);
    }
    // Tera type only (Adaptability applies).
    else if (ctx->moveType == teraType && !IS_BATTLER_OF_BASE_TYPE(ctx->battlerAtk, ctx->moveType))
    {
        if (ctx->abilityAtk == ABILITY_ADAPTABILITY)
            return UQ_4_12(2.0);
        else
            return UQ_4_12(1.5);
    }
    // Base type only (Adaptability does not apply while Terastallized).
    else if (ctx->moveType != teraType && IS_BATTLER_OF_BASE_TYPE(ctx->battlerAtk, ctx->moveType))
    {
        return UQ_4_12(1.5);
    }
    // Neither base or Tera type.
    else
    {
        return UQ_4_12(1.0);
    }
}

u16 GetTeraTypeRGB(enum Type type)
{
    return gTypesInfo[type].teraTypeRGBValue;
}
