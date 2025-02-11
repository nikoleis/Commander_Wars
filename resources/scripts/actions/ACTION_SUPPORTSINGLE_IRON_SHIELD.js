var Constructor = function()
{
    this.canBePerformed = function(action)
    {
        var unit = action.getTargetUnit();
        var actionTargetField = action.getActionTarget();
        var targetField = action.getTarget();
        if ((unit.getHasMoved() === true) ||
            (unit.getBaseMovementCosts(actionTargetField.x, actionTargetField.y) <= 0))
        {
            return false;
        }
        if (((actionTargetField.x === targetField.x) && (actionTargetField.y === targetField.y) ||
            (action.getMovementTarget() === null)) &&
            (ACTION_SUPPORTSINGLE_IRON_SHIELD.getDefenseFields(action).length > 0))
        {
            return true;
        }
        else
        {
            return false;
        }
    };
    this.getActionText = function()
    {
        return qsTr("Iron Shield");
    };
    this.getIcon = function()
    {
        return "defenseStar";
    };
    this.getStepInputType = function(action)
    {
        return "FIELD";
    };
    this.getStepData = function(action, data)
    {
        var unit = action.getTargetUnit();
        var actionTargetField = action.getActionTarget();
        data.setColor("#C800FF00");
        var fields = ACTION_SUPPORTSINGLE_IRON_SHIELD.getDefenseFields(action);
        for (var i3 = 0; i3 < fields.length; i3++)
        {
            data.addPoint(Qt.point(fields[i3].x, fields[i3].y));
        }
    };
    this.getDefenseFields = function(action)
    {
        var targetField = action.getActionTarget();
        var targetFields = [Qt.point(targetField.x + 1, targetField.y),
                            Qt.point(targetField.x - 1, targetField.y),
                            Qt.point(targetField.x,     targetField.y - 1),
                            Qt.point(targetField.x,     targetField.y + 1)];
        // check all neighbour terrains
        var unit = action.getTargetUnit();
        var ret = [];
        for (var i = 0; i < targetFields.length; i++)
        {
            if (map.onMap(targetFields[i].x, targetFields[i].y))
            {
                var terrain = map.getTerrain(targetFields[i].x, targetFields[i].y);
                var repairUnit = terrain.getUnit();
                // can we repair the unit?
                if (repairUnit !== null &&
                    repairUnit.getOwner() === unit.getOwner() &&
                    repairUnit !== unit)
                {
                    ret.push(targetFields[i]);
                }
            }
        }
        return ret;
    };

    this.isFinalStep = function(action)
    {
        if (action.getInputStep() === 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
    this.postAnimationUnit = null;
    this.postAnimationTargetX = -1;
    this.postAnimationTargetY = -1;
    this.perform = function(action)
    {
        // we need to move the unit to the target position
        var unit = action.getTargetUnit();
        var animation = Global[unit.getUnitID()].doWalkingAnimation(action);
        animation.setEndOfAnimationCall("ACTION_SUPPORTSINGLE_IRON_SHIELD", "performPostAnimation");
        // move unit to target position
        unit.moveUnitAction(action);
        // disable unit commandments for this turn
        action.startReading();
        // read action data
        ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationUnit = unit;
        ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetX = action.readDataInt32();
        ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetY = action.readDataInt32();
    };
    this.performPostAnimation = function(postAnimation)
    {
        var terrain = map.getTerrain(ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetX, ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetY);
        var defenseUnit = terrain.getUnit();
        var animation = GameAnimationFactory.createAnimation(ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetX, ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetY);
        var width = animation.addText(qsTr("DEFENSE"), map.getImageSize() / 2 + 25, 2, 1);
        animation.addBox("info", map.getImageSize() / 2, 0, width + 36, map.getImageSize(), 400);
        animation.addSprite("defense", map.getImageSize() / 2 + 4, 4, 400, 2);
        defenseUnit.addDefensiveBonus(200);
        var playerId  = ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationUnit.getOwner().getPlayerID();
        defenseUnit.loadIcon("iron_shield", map.getImageSize() / 2, map.getImageSize() / 2, 1, playerId)
        ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationUnit.setHasMoved(true);
        ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationUnit = null;
        ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetX = -1;
        ACTION_SUPPORTSINGLE_IRON_SHIELD.postAnimationTargetY = -1;
    };
    this.getDescription = function()
    {
        return qsTr("Shields an allied unit. This highly increases the defense of the unit until the start of the next turn.");
    };
}

Constructor.prototype = ACTION;
var ACTION_SUPPORTSINGLE_IRON_SHIELD = new Constructor();
