// this is the base class for terrain
var BUILDING =
{
    // loader for stuff which needs C++ Support
    init : function (building)
    {
        building.setVisionHigh(1);
    },
    getName : function(building)
    {
        return "";
    },
    // returns the defense of this terrain
    getDefense : function(building)
    {
        return 3;
    },

    // vision bonus for units created by this building
    getVisionBonus : function(building)
    {
        return 0;
    },
    // additional offensive bonus for a unit on this field
    getOffensiveFieldBonus : function(co, attacker, atkPosX, atkPosY,
                                      defender, defPosX, defPosY, isDefender, action, luckMode)
    {
        return 0;
    },
    //  additional deffensive bonus for a unit on this field
    getDeffensiveFieldBonus : function(co, attacker, atkPosX, atkPosY,
                                       defender, defPosX, defPosY, isDefender, action, luckMode)
    {
        return 0;
    },
    getBuildingWidth : function()
    {
        // one field width default for most buildings
        return 1;
    },
    getBuildingHeigth : function()
    {
        // one field heigth default for most buildings
        return 1;
    },
    loadSprites : function(building, neutral)
    {
    },
    // the terrain on which a building can be placed
    // if the current terrain isn't in the list. it'll be replaced by the first :)
    baseTerrains : ["PLAINS", "STREET", "STREET1", "SNOW", "SNOW_STREET", "DESERT", "DESERT_PATH", "DESERT_PATH1", "WASTE", "WASTE_PATH1"],
    getBaseTerrain : function(building)
    {
        return Global[building.getBuildingID()].baseTerrains;
    },

    addCaptureAnimationBuilding : function(animation, building, startPlayer, capturedPlayer)
    {
        animation.addBuildingSprite("town+mask", startPlayer , capturedPlayer, GameEnums.Recoloring_Matrix);
    },

    canLargeBuildingPlaced : function(terrain, building, width, heigth)
    {
        var placeX = terrain.getX();
        var placeY = terrain.getY();
        var baseTerrains = building.getBaseTerrain();
        for (var x = 0; x < width; x++)
        {
            for (var y = 0; y < heigth; y++)
            {
                if (map.onMap(placeX - x, placeY - y))
                {
                    if (baseTerrains.indexOf(map.getTerrain(placeX - x, placeY - y).getTerrainID()) < 0)
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
        return true;
    },

    canBuildingBePlaced : function(terrain, building)
    {
        var baseTerrains = building.getBaseTerrain();
        if (baseTerrains.indexOf(terrain.getTerrainID()) >= 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    },

    getBaseIncome : function(building)
    {
        return 1000;
    },

    getConstructionList : function(building)
    {
        return [];
    },
    actionList : [],
    getActions : function(building)
    {
        return Global[building.getBuildingID()].actionList;
    },
    startOfTurn : function(building)
    {
        var owner = building.getOwner();
        if (owner !== null)
        {
            if (!owner.getIsDefeated())
            {
                BUILDING.replenishUnit(building);
            }
        }
    },

    getOffensiveBonus : function(building)
    {
        return 0;
    },

    getDefensiveBonus : function(building)
    {
        return 0;
    },
    getActionTargetFields : function(building)
    {
        // targets of a building. For most things this is a null pointer
        // return must be null or a QmlVectorPoint
        return null;
    },
    getActionTargetOffset : function(building)
    {
        // offset for large buildings since there reference point is bound to the lower right corner.
        return Qt.point(0, 0);
    },
    getIsAttackable : function(building, x, y)
    {
        return true;
    },

    getRepairTypes : function(building)
    {
        return[];
    },

    replenishUnit : function(building)
    {
        // default impl replenishes our units
        // gets called at the start of a turn
        var constructionList = building.getConstructionList();
        var repairList = building.getRepairTypes();
        var unit = building.getTerrain().getUnit();
        if ((unit !== null) &&
                (unit.getOwner() === building.getOwner()) &&
                ((repairList.indexOf(unit.getUnitType()) >= 0) ||
                 (constructionList.indexOf(unit.getUnitID()) >= 0)))
        {
            var x = unit.getX();
            var y = unit.getY();
            if (unit.canBeRepaired(Qt.point(x, y)))
            {
                BUILDING.repairUnit(unit, x, y);
            }
        }
    },

    repairUnit : function(unit, x, y)
    {
        // our unit and a repairable one
        // replenish it
        var refillRule = map.getGameRules().getGameRule("GAMERULE_REFILL_MATERIAL");
        var refillMaterial = (typeof refillRule === 'undefined' || refillRule === null); // an existing rule equals it's set
        unit.refill(refillMaterial);
        var repairAmount = 2 + unit.getRepairBonus(Qt.point(x, y));
        UNIT.repairUnit(unit, repairAmount);
        if (!unit.isStealthed(map.getCurrentViewPlayer()))
        {
            var animationCount = GameAnimationFactory.getAnimationCount();
            var animation = GameAnimationFactory.createAnimation(x, y);
            var width = animation.addText(qsTr("REPAIR"), map.getImageSize() / 2 + 25, 2, 1);
            animation.addBox("info", map.getImageSize() / 2, 0, width + 36, map.getImageSize(), 400);
            animation.addSprite("repair", map.getImageSize() / 2 + 4, 4, 400, 2);
            animation.addSound("repair_2.wav");
            if (animationCount > 0)
            {
                GameAnimationFactory.getAnimation(animationCount - 1).queueAnimation(animation);
            }
        }
    },

    getMiniMapIcon : function(building)
    {
        return "minimap_building";
    },

    onDestroyed : function(building)
    {
        // called when the building is destroyed and replacing of this building starts
    },

    getDamage : function(building, unit)
    {
        return 0;
    },

    getBuildingTargets : function(building)
    {
        // hint for the ai
        return GameEnums.BuildingTarget_Own;
    },

    getTerrainAnimationMoveSpeed : function()
    {
        return 0;
    },

    getTerrainAnimationBase : function(unit, terrain)
    {
        var weatherModifier = TERRAIN.getWeatherModifier();
        return "base_" + weatherModifier + "air";
    },

    getTerrainAnimationForeground : function(unit, terrain)
    {
        return "";
    },


    armyData = [["ac", "yc"],
                ["os", "os"],
                ["bm", "bm"],
                ["ge", "ge"],
                ["yc", "yc"],
                ["gs", "yc"],
                ["ti", "ge"],
                ["dm", "ge"],
                ["pf", "os"],
                ["bd", "bm"],],

    getTerrainAnimationBackground : function(unit, terrain)
    {
        var variables = terrain.getVariables();
        var variable = variables.getVariable("BACKGROUND_ID");
        var armyVariable = variables.getVariable("ARMYBACKGROUND_ID");
        var rand = 0;
        var randArmy = 0;
        if (variable === null)
        {
            rand = globals.randInt(0, 1);
            variable = variables.createVariable("BACKGROUND_ID");
            armyVariable = variables.createVariable("ARMYBACKGROUND_ID");
            variable.writeDataInt32(rand);
            randArmy = globals.randInt(0, BUILDING.armyData.length - 1);
            armyVariable.writeDataInt32(randArmy);
        }
        else
        {
            rand = variable.readDataInt32();
            randArmy = armyVariable.readDataInt32();
        }
        var baseId = terrain.getBaseTerrainID();
        var building = terrain.getBuilding();
        var player = building.getOwner();

        var army = BUILDING.armyData[randArmy][1];
        if (player !== null)
        {
            army = Global.getArmyNameFromPlayerTable(player, BUILDING.armyData);
        }
        var weatherModifier = TERRAIN.getWeatherModifier();
        if (baseId === "DESERT" ||
            weatherModifier === "desert")
        {
            return "back_deserttown";
        }
        else if (baseId === "WASTE")
        {
            return "back_wastetown";
        }

        return "back_" + weatherModifier + "town+" + army + "+" + rand.toString();
    },

    getDescription : function(building)
    {
        return "";
    },

    // vision created by this field
    getVision : function(building)
    {
        return 0;
    },

    getVisionHide : function(building)
    {
        return false;
    },

    onCaptured : function(building)
    {
    },

    onWeatherChanged : function(building, weather)
    {
        // called when the weather changes
        // call loadWeatherOverlaySpriteV2 to load an sprite overlay
    },
};
