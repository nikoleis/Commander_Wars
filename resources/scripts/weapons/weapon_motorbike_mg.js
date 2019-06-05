var Constructor = function()
{
    this.getName = function()
    {
        return qsTr("MG");
    };
    this.getEnviromentDamage = function(enviromentId)
    {
        return 1;
    };
    this.getBaseDamage = function(unit)
    {
        switch(unit.getUnitID())
        {
        case "INFANTRY":
            return 65;
        case "MECH":
            return 55;
        case "MOTORBIKE":
            return 55;
        case "SNIPER":
            return 75;

        case "APC":
            return 20;
        case "FLARE":
            return 20;
        case "RECON":
            return 20;

        case "FLAK":
            return 5;
        case "HOVERFLAK":
            return 5;
        case "LIGHT_TANK":
            return 5;
        case "HOVERCRAFT":
            return 5;

        case "HEAVY_HOVERCRAFT":
            return 3;
        case "HEAVY_TANK":
            return 3;
        case "NEOTANK":
            return 3;

        case "MEGATANK":
            return 1;

        case "HOELLIUM":
            return 20;

        case "T_HELI":
            return 30;
        case "K_HELI":
            return 10;

        case "ARTILLERY":
            return 15;
        case "ANTITANKCANNON":
            return 45;
        case "MISSILE":
            return 35;
        case "ROCKETTHROWER":
            return 35;
        case "PIPERUNNER":
            return 6;
        default:
            return -1;
        }
    };
};

Constructor.prototype = WEAPON;
var WEAPON_MOTORBIKE_MG = new Constructor();
