var Constructor = function()
{
    this.getMaxUnitCount = function()
    {
        return 5;
    };
    this.loadMoveInAnimation = function(sprite, unit, defender, weapon)
    {
        var count = sprite.getUnitCount(BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.getMaxUnitCount());
        var startX = -105;
        sprite.loadMovingSpriteV2("tank_hunter+mask", GameEnums.Recoloring_Matrix, sprite.getMaxUnitCount(), Qt.point(startX, 5),
                                  Qt.point(65, 0), 600, false,
                                  1, 1);
        sprite.loadMovingSprite("vehicle_dust", false, sprite.getMaxUnitCount(), Qt.point(startX - 20, 7),
                                Qt.point(65, 0), 600, false,
                                1, 1);
        for (var i = 0; i < count; i++)
        {
            sprite.loadSound("heavy_tank_move.wav", 5, i * BATTLEANIMATION.defaultFrameDelay);
        }
        BATTLEANIMATION.loadSpotterOrCoMini(sprite, unit, false);
    };

    this.loadStopAnimation = function(sprite, unit, defender, weapon)
    {
        BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.loadSprite(sprite, unit, defender, weapon, "", 1);
        sprite.loadSprite("vehicle_dust_stop",  false,
                          BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.getMaxUnitCount(), Qt.point(-60, 7), 1);
    };

    this.loadStandingAnimation = function(sprite, unit, defender, weapon)
    {
        BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.loadSprite(sprite, unit, defender, weapon, "", 1);
    };

    this.loadSprite = function(sprite, unit, defender, weapon, ending, count)
    {
        var offset = Qt.point(-40, 5);
        sprite.loadSpriteV2("tank_hunter" + ending + "+mask", GameEnums.Recoloring_Matrix,
                            BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.getMaxUnitCount(), offset, count);
        BATTLEANIMATION.loadSpotterOrCoMini(sprite, unit, false);
    };

    this.loadFireAnimation = function(sprite, unit, defender, weapon)
    {
        var offset = Qt.point(0, 0);
        var count = sprite.getUnitCount(BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.getMaxUnitCount());
        if (weapon === 0)
        {
            BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.loadSprite(sprite, unit, defender, weapon, "", 1);
            offset = Qt.point(42, 18);
            sprite.loadSprite("medium_shot",  false, sprite.getMaxUnitCount(), offset,
                              1, 1.0, 0, 120);
            for (var i = 0; i < count; i++)
            {
                sprite.loadSound("tank_shot.wav", 1, 120 + i * BATTLEANIMATION.defaultFrameDelay);
            }
        }
        else
        {
            BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.loadSprite(sprite, unit, defender, weapon, "", 1);
            // mg
            offset = Qt.point(18, 43);
            sprite.loadSprite("mg_shot",  false, sprite.getMaxUnitCount(), offset,
                              1, 1, 0, 0);
            for (var i = 0; i < count; i++)
            {
                sprite.loadSound("mg_weapon_fire.wav", 1, i * BATTLEANIMATION.defaultFrameDelay);
                sprite.loadSound("mg_weapon_fire.wav", 1, 200 + i * BATTLEANIMATION.defaultFrameDelay);
                sprite.loadSound("mg_weapon_fire.wav", 1, 400 + i * BATTLEANIMATION.defaultFrameDelay);
            }
        }
    };

    this.getFireDurationMS = function(sprite, unit, defender, weapon)
    {
        return 600 + BATTLEANIMATION.defaultFrameDelay * sprite.getUnitCount(BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.getMaxUnitCount());
    };

    this.hasMoveInAnimation = function(sprite, unit, defender, weapon)
    {
        return true;
    };
    this.getMoveInDurationMS = function(sprite, unit, defender, weapon)
    {
        return 610;
    };

    this.getStopDurationMS = function(sprite, unit, defender, weapon)
    {
        return 300 + BATTLEANIMATION.defaultFrameDelay * BATTLEANIMATION_ZCOUNIT_TANK_HUNTER.getMaxUnitCount();
    };
};

Constructor.prototype = BATTLEANIMATION;
var BATTLEANIMATION_ZCOUNIT_TANK_HUNTER = new Constructor();
