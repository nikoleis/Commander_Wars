CO_WAYLON.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                       defender, defPosX, defPosY, isDefender, action)
{
    if (co.getIsCO0() === true)
    {
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            if (attacker.getUnitType() === GameEnums.UnitType_Air)
            {
                return 50;
            }
            return 0;
        case GameEnums.PowerMode_Power:
            if (attacker.getUnitType() === GameEnums.UnitType_Air)
            {
                return 10;
            }
            return 0;
        default:
            break;
        }
    }
    return 0;
};

CO_WAYLON.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                        defender, defPosX, defPosY, isAttacker, action)
{
    if (co.getIsCO0() === true)
    {
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            if (defender.getUnitType() === GameEnums.UnitType_Air)
            {
                return 270;
            }
            break;
        case GameEnums.PowerMode_Power:
            if (defender.getUnitType() === GameEnums.UnitType_Air)
            {
                return 200;
            }
            else
            {
                return 0;
            }
        default:
            if (defender.getUnitType() === GameEnums.UnitType_Air)
            {
                return 30;
            }
            break;
        }
    }
    return 0;
};
