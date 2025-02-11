var Constructor = function()
{
    // loader for stuff which needs C++ Support
    this.init = function (terrain)
    {
        terrain.setVisionHigh(1);
        terrain.setTerrainName(__BASEMOUNTAIN.getName(terrain));
    };
    this.baseTerrainId = "PLAINS";
    this.getName = function(terrain = null)
    {
        var baseTerrainId = ""
        if (terrain !== null)
        {
            baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
        }
        if (baseTerrainId === "WASTE")
        {
            return qsTr("Waste Rock");
        }
        else if (baseTerrainId === "SNOW")
        {
            return qsTr("Snowy Mountain");
        }
        else if (baseTerrainId === "DESERT")
        {
            return qsTr("Rock");
        }
        else
        {
            return qsTr("Mountain");
        }
    };
    this.getDefense = function(terrain)
    {
        var baseTerrainId = ""
        if (terrain !== null)
        {
            baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
        }
        if (baseTerrainId === "WASTE")
        {
            return 3;
        }
        else
        {
            return 4;
        }
    };
    this.getOffensiveFieldBonus = function(terrain, attacker, atkPosX, atkPosY,
                                           defender, defPosX, defPosY, isDefender, action, luckMode)
    {
        var baseTerrainId = ""
        if (terrain !== null)
        {
            baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
        }
        if (baseTerrainId === "DESERT")
        {
            return -20;
        }
        return 0;
    };
    this.loadBaseTerrain = function(terrain, currentTerrainID)
    {
        if (currentTerrainID === "SNOW")
        {
            terrain.loadBaseTerrain("SNOW");
        }
        else if (currentTerrainID === "DESERT")
        {
            terrain.loadBaseTerrain("DESERT");
        }
        else if (currentTerrainID === "WASTE")
        {
            terrain.loadBaseTerrain("WASTE");
        }
        else if (currentTerrainID === "PLAINS")
        {
            terrain.loadBaseTerrain("PLAINS");
        }
        else
        {
            var baseTerrainId = ""
            if (terrain !== null)
            {
                baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
            }
            terrain.loadBaseTerrain(baseTerrainId);
        }
    };

    this.loadBase = function(terrain, spriteId)
    {
        var surroundings = terrain.getSurroundings("MOUNTAIN,DESERT_ROCK,SNOW_MOUNTAIN,WASTE_MOUNTAIN", false, false, GameEnums.Directions_Direct, false);
        var itemCount = surroundings.split("+").length - 1;
        if (itemCount === 4)
        {
            terrain.loadBaseSprite(spriteId);
        }
        else
        {
            terrain.loadBaseSprite(spriteId + "+short");
        }
    };
    this.getMiniMapIcon = function(terrain)
    {
        var baseTerrainId = ""
        if (terrain !== null)
        {
            baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
        }
        if (baseTerrainId === "WASTE")
        {
            return "minimap_waste_mountain";
        }
        else if (baseTerrainId === "SNOW")
        {
            return "minimap_snow_mountain";
        }
        else if (baseTerrainId === "DESERT")
        {
            return "minimap_desert_rock";
        }
        else
        {
            return "minimap_mountain";
        }
    };
    this.getBonusVision = function(unit, terrain)
    {
        var baseTerrainId = ""
        if (terrain !== null)
        {
            baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
        }
        if (unit.getUnitType() === GameEnums.UnitType_Infantry)
        {
            if (baseTerrainId === "WASTE")
            {
                return 4;
            }
            return 3;
        }
        else
        {
            if (baseTerrainId === "WASTE")
            {
                return 1;
            }
            return 0;
        }
    };

    this.getDescription = function(terrain)
    {
        var baseTerrainId = ""
        if (terrain !== null)
        {
            baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
        }
        if (baseTerrainId === "WASTE")
        {
            return qsTr("<r>Clear view. In Fog of War, Infantry unit's gain </r><div c='#00ff00'>vision +4.</div><r> Extremly high movement costs for infantry units. Waste terrain with reduced defense but clear view. In Fog of War, other unit's gain </r><div c='#00ff00'>vision +1.</div>");
        }
        else if (baseTerrainId === "SNOW")
        {
            return qsTr("<r>Clear view. In Fog of War, Infantry unit's gain </r><div c='#00ff00'>vision +3.</div><r> Extremly high movement costs for infantry units.</r>");
        }
        else if (baseTerrainId === "DESERT")
        {
            return "<r>" + qsTr("Clear view. In Fog of War, Infantry unit's gain ") + "</r>" +
                    "<div c='#00ff00'>" + qsTr("vision +3.") + "</div>" +
                    "<r>" + qsTr(" It reduces the firepower of units by 20%.") + "</r>"
        }
        else
        {
            return qsTr("<r>Clear view. In Fog of War, Infantry unit's gain </r><div c='#00ff00'>vision +3.</div>");
        }
    };
    this.getSprites = function(spriteId)
    {
        return [spriteId,
                spriteId + "+short"];
    };
    this.getTerrainAnimationForeground = function(unit, terrain)
    {
        return "";
    };
    this.getTerrainAnimationBackground = function(unit, terrain)
    {
        var variables = terrain.getVariables();
        var variable = variables.getVariable("BACKGROUND_ID");
        var rand = 0;
        if (variable === null)
        {
            rand = globals.randInt(0, 1);
            variable = variables.createVariable("BACKGROUND_ID");
            variable.writeDataInt32(rand);
        }
        else
        {
            rand = variable.readDataInt32();
        }
        var baseTerrainId = ""
        if (terrain !== null)
        {
            baseTerrainId = Global[terrain.getTerrainID()].baseTerrainId;
        }
        if (baseTerrainId === "WASTE")
        {
            return "back_wastemountain";
        }
        else if (baseTerrainId === "SNOW")
        {
            return "back_snowmountain+" + rand.toString();
        }
        else if (baseTerrainId === "DESERT")
        {
            return "back_desertmountain+" + rand.toString();
        }
        else
        {
            var weatherModifier = TERRAIN.getWeatherModifier();
            return "back_" + weatherModifier + "mountain+" + rand.toString();
        }
    };
};
Constructor.prototype = TERRAIN;
var __BASEMOUNTAIN = new Constructor();
