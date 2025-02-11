var CO =
{
    init : function(co)
    {
        co.setPowerStars(3);
        co.setSuperpowerStars(3);
    },

    onCOUnitLost : function(co)
    {
        // called when a co unit got destroyed
        var gamerules = map.getGameRules();
        co.setPowerFilled(co.getPowerFilled() * (1 - gamerules.getPowerLoose()));
    },

    loadCOMusic : function(co)
    {
        // put the co music in here.
    },

    getMovementpointModifier : function(co, unit, posX, posY)
    {
        return 0;
    },

    buildedUnit : function(co, unit)
    {
        // called when someone builded a unit -> ACTION_BUILD_UNITS was performed
    },

    getFirerangeModifier : function(co, unit, posX, posY)
    {
        return 0;
    },

    getMinFirerangeModifier : function(co, unit, posX, posY)
    {
        return 0;
    },

    getCaptureBonus : function(co, unit, posX, posY)
    {
        return 0;
    },

    getAttackHpBonus : function(co, unit, posX, posY)
    {
        return 0;
    },

    getVisionrangeModifier: function(co, unit, posX, posY)
    {
        return 0;
    },

    getTerrainDefenseModifier : function(co, unit, posX, posY)
    {
        return 0;
    },

    getFirstStrike : function(co, unit, posX, posY, attacker, isDefender)
    {
        return false;
    },

    getEnemyTerrainDefenseModifier : function(co, unit, posX, posY)
    {
        return 0;
    },

    getDamageReduction : function(co, damage, attacker, atkPosX, atkPosY, attackerBaseHp,
                                  defender, defPosX, defPosY, isDefender, luckMode)
    {
        return 0;
    },

    canCounterAttack : function(co, attacker, atkPosX, atkPosY,
                                defender, defPosX, defPosY, luckMode)
    {
        return GameEnums.CounterAttackMode_Undefined;
    },

    getTrueDamage : function(co, damage, attacker, atkPosX, atkPosY, attackerBaseHp,
                             defender, defPosX, defPosY, isDefender, action, luckMode)
    {
        return 0;
    },

    getPerfectVision : function(co)
    {
        return false;
    },

    getWeatherImmune : function(co)
    {
        // return true if a weather has no effect for the co
        return false;
    },

    getHpHidden : function(co, unit, posX, posY)
    {
        // are the hp hidden of this unit?
        return false;
    },

    getRankInfoHidden : function(co, unit, posX, posY)
    {
        // are the hp hidden of this unit?
        return false;
    },

    getPerfectHpView : function(co, unit, posX, posY)
    {
        // are the co zone and rank hidden?
        return false;
    },

    getRepairBonus : function(co, unit, posX, posY)
    {
        return 0;
    },

    getBonusLuck : function(co, unit, posX, posY)
    {
        return 0;
    },

    getBonusMisfortune : function(co, unit, posX, posY)
    {
        return 0;
    },

    getActionModifierList : function(co, unit)
    {
        // return empty list as default
        return [];
    },

    activatePower : function(co)
    {
    },

    activateSuperpower : function(co)
    {
    },

    getFuelCostModifier : function(co, unit, posX, posY, costs)
    {
        // modifies the fuel cost at the start of a turn
        return 0;
    },

    getOffensiveBonus : function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender, action, luckMode)
    {
        return 0;
    },

    getOffensiveReduction : function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender, action, luckMode)
    {
        return 0;
    },

    getDeffensiveBonus : function(co, attacker, atkPosX, atkPosY,
                                  defender, defPosX, defPosY, isAttacker, action, luckMode)
    {
        return 0;
    },

    getDeffensiveReduction : function(co, attacker, atkPosX, atkPosY,
                                  defender, defPosX, defPosY, isAttacker, action, luckMode)
    {
        return 0;
    },

    canBeRepaired : function(co, unit, posX, posY)
    {
        // called from all co's for a unit -> so you can apply boni from own co and mali from enemy co's here
        return true;
    },

    getCostModifier : function(co, id, baseCost, posX, posY)
    {
        return 0;
    },

    getEnemyCostModifier : function(co, id, baseCost, posX, posY)
    {
        return 0;
    },

    getMovementcostModifier : function(co, unit, posX, posY)
    {
        // called from all co's for a unit -> so you can apply boni from own co and mali from enemy co's here
        return 0;
    },

    getMovementFuelCostModifier : function(co, unit, fuelCost)
    {
        // modifies the fuel cost when moving
        // called from all co's for a unit -> so you can apply boni from own co and mali from enemy co's here
        // fuelCost are the costs needed for the current movement
        return 0;
    },

    getCanMoveAndFire : function(co, unit, posX, posY)
    {
        return false;
    },

    getCOArmy : function()
    {
        return "OS";
    },

    getStarGain : function(co, fundsDamage, x, y, hpDamage, defender, counterAttack)
    {
        var gamerules = map.getGameRules();
        var powerCostIncrease = gamerules.getPowerUsageReduction();
        var multiplier = 1 / (1.0 + co.getPowerUsed() * powerCostIncrease);
        var gainMode = gamerules.getPowerGainMode();
        var gainZone = gamerules.getPowerGainZone();
        var baseValue = 0;
        // select gain value
        if (gainMode === GameEnums.PowerGainMode_Money)
        {
            baseValue = fundsDamage / 9000;
            if (!defender)
            {
                // reduce damage for attacker
                baseValue *= 0.5;
            }
        }
        else if (gainMode === GameEnums.PowerGainMode_Money_OnlyAttacker)
        {
            if (!defender)
            {
                // only charge for attacker
                baseValue = fundsDamage / 9000;
            }
        }
        else if (gainMode === GameEnums.PowerGainMode_Hp)
        {
            baseValue = hpDamage / 10.0;
            if (!defender)
            {
                // reduce damage for attacker
                baseValue *= 0.5;
            }
        }
        else if (gainMode === GameEnums.PowerGainMode_Hp_OnlyAttacker)
        {
            if (!defender)
            {
                // only charge for attacker
                baseValue = hpDamage / 10.0;
            }
        }
        var powerGain = baseValue * multiplier;
        if (gainZone === GameEnums.PowerGainZone_Global)
        {
            // do nothing
        }
        else if (gainZone === GameEnums.PowerGainZone_GlobalCoZoneBonus)
        {
            if (!co.inCORange(Qt.point(x, y), null))
            {
                // reduce power meter gain when not in co range
                powerGain *= 0.5;
            }
        }
        else if (gainZone === GameEnums.PowerGainZone_OnlyCoZone)
        {
            if (!co.inCORange(Qt.point(x, y), null))
            {
                // no power gain outside co-zone
                powerGain = 0;
            }
        }
        return powerGain;
    },

    gainPowerstar : function(co, fundsDamage, x, y, hpDamage, defender, counterAttack)
    {
        var powerGain = CO.getStarGain(co, fundsDamage, x, y, hpDamage, defender, counterAttack)
        co.setPowerFilled(co.getPowerFilled() + powerGain)
    },

    getCOUnitRange : function(co)
    {
        return 0;
    },

    getCOUnits : function(co, building)
    {
        return [];
    },

    getEnemyVisionBonus : function (co, unit, x, y)
    {
        return 0;
    },

    getEnemyMinFirerangeModifier : function (co, unit, x, y)
    {
        return 0;
    },

    getEnemyFirerangeModifier : function (co, unit, x, y)
    {
        return 0;
    },

    getBonusLoadingPlace : function (co, unit, x, y)
    {
        return 0;
    },

    getAdditionalBuildingActions : function(co, building)
    {
        // called from all co's for a building -> so you can boni from own co and mali from enemy co's here
        // - before an action id will disable the action -> see Mary
        return "";
    },

    getTransportUnits : function(co, unit)
    {
        // called to check for additional loading units for a transporter
        // - before an unit id will remove the unit from the loading list
        return [];
    },

    getBonusIncome : function(co, building, income)
    {
        return 0;
    },

    getIncomeReduction : function(co, building, income)
    {
        return 0;
    },

    postBattleActions : function(co, attacker, atkDamage, defender, gotAttacked, weapon, action)
    {
        // called after damage was dealt to the defender unit.
        // the damage given is the damage was dealt to the unit.
        // gotAttacked means we own the unit which got damage dealt.
    },

    startOfTurn : function(co)
    {
        // called at the start of the turn use it to do cool co stuff like caulder's healing :)
    },

    postAction: function(co, action)
    {
        // called after the action was performed
    },

    onUnitDeath : function(co, unit)
    {
    },

    // CO - Intel
    getBio : function(co)
    {
        return "";
    },
    getLongBio : function(co)
    {
        return "";
    },
    getHits : function(co)
    {
        return "";
    },
    getMiss : function(co)
    {
        return "";
    },
    getCODescription : function(co)
    {
        return "";
    },
    getLongCODescription : function(co)
    {
        return "";
    },
    getPowerDescription : function(co)
    {
        return "";
    },
    getPowerName : function(co)
    {
        return "";
    },
    getSuperPowerDescription : function(co)
    {
        return "";
    },
    getSuperPowerName : function(co)
    {
        return "";
    },
    getPowerSentences : function(co)
    {
        return [];
    },
    getVictorySentences : function(co)
    {
        return [];
    },
    getDefeatSentences : function(co)
    {
        return [];
    },
    getName : function(co)
    {
        return "";
    },

    showDefaultUnitGlobalBoost : function(co)
    {
        return true;
    },
    getCustomUnitGlobalBoostCount : function(co)
    {
        return 0;
    },
    getCustomUnitGlobalBoost : function(co, index, info)
    {
    },
    showDefaultUnitZoneBoost : function(co)
    {
        return true;
    },
    getCustomUnitZoneBoostCount : function(co)
    {
        return 0;
    },
    getCustomUnitZoneBoost : function(co, index, info)
    {
    },

    getCOStyles : function()
    {
        // string array containing the endings of the alternate co style
        
        return [];
    },

    // ai hints for using co powers
    /**
      * co              : getting checked
      * powerSurplus    : surplus on the co power e.g 1 if one more star is filled over the normal co power
      * unitCount       : amount of units owned by the player
      * repairUnits     : amount of units that need to be repaired
      * indirectUnits   : amount of indirect units
      * directUnits     : amount of direct units
      * enemyUnits      : amount of enemy units
      * turnMode        : see GameEnums.h AiTurnMode describes the current turn mode on the ai
      * return          : see GameEnums PowerMode return unknown for default fallback -> not recommended
      */
    getAiUsePower(co, powerSurplus, unitCount, repairUnits, indirectUnits, directUnits, enemyUnits, turnMode)
    {
        return CO.getAiUsePowerAtStart(co, powerSurplus, turnMode);
    },

//    getAiCoUnitBonus : function(co, unit)
//    {
//        implement this function for a co to make the ai build more of the co's good units and to increase the chance
//        the ai deploys a co unit in it. The return value is capped at 10 and -10
//        return 0;
//    },

    getAiCoBuildRatioModifier : function(co)
    {
        // multiplier shifting the general indirect to direct unit ratio the ai tries to maintain.
        return 1.0;
    },

    getAiUsePowerAlways : function(co, powerSurplus)
    {
        if (co.canUseSuperpower())
        {
            return GameEnums.PowerMode_Superpower;
        }
        else if (powerSurplus <= 0.5 &&
                 co.canUsePower())
        {
            return GameEnums.PowerMode_Power;
        }
        return GameEnums.PowerMode_Off;
    },

    getAiUsePowerAtStart : function(co, powerSurplus, turnMode)
    {
        if (turnMode === GameEnums.AiTurnMode_StartOfDay)
        {
            if (co.canUseSuperpower())
            {
                return GameEnums.PowerMode_Superpower;
            }
            else if (powerSurplus <= 0.5 &&
                     co.canUsePower())
            {
                return GameEnums.PowerMode_Power;
            }
        }
        return GameEnums.PowerMode_Off;
    },

    getAiUsePowerAtEnd : function(co, powerSurplus, turnMode)
    {
        if (turnMode === GameEnums.AiTurnMode_EndOfDay)
        {
            if (co.canUseSuperpower())
            {
                return GameEnums.PowerMode_Superpower;
            }
            else if (powerSurplus <= 0.5 &&
                     co.canUsePower())
            {
                return GameEnums.PowerMode_Power;
            }
        }
        return GameEnums.PowerMode_Off;
    },

    getAiUsePowerAtUnitCount : function(co, powerSurplus, turnMode, unitCount, count)
    {
        if (turnMode === GameEnums.AiTurnMode_StartOfDay &&
            unitCount >= 5)
        {
            if (co.canUseSuperpower())
            {
                return GameEnums.PowerMode_Superpower;
            }
            else if (powerSurplus <= 0.5 &&
                     co.canUsePower())
            {
                return GameEnums.PowerMode_Power;
            }
        }
        return GameEnums.PowerMode_Off;
    },

    getUnitBuildValue : function(co, unitID)
    {
        return 0.0;
    },

    getAddtionalCoFaces : function()
    {
        return ["co_fanatic",
                "co_cyrus",
                "co_dr_morris",
                "co_major",
                "co_civilian1",
                "co_civilian2",
                "co_civilian3",
                "co_civilian4",
                "co_civilian5",
                "co_civilian6",
                "co_civilian7",
                "co_civilian8",
                "co_officier_os",
                "co_officier_bm",
                "co_officier_ge",
                "co_officier_yc",
                "co_officier_bh",
                "co_officier_ac",
                "co_officier_bd",
                "co_officier_ti",
                "co_officier_dm",];
    },
}

