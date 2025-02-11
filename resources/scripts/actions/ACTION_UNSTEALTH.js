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
        if ((actionTargetField.x === targetField.x) && (actionTargetField.y === targetField.y) ||
            (action.getMovementTarget() === null))
        {
            if (unit.getHidden() === true)
            {
                return true;
            }
        }
        return false;
    };
    this.getActionText = function()
    {
        return qsTr("Unstealth");
    };
    this.getIcon = function()
    {
        return "unhide";
    };
    this.isFinalStep = function(action)
    {
        return true;
    };
    this.perform = function(action)
    {
        // we need to move the unit to the target position
        var unit = action.getTargetUnit();
        var animation = Global[unit.getUnitID()].doWalkingAnimation(action);
        animation.setEndOfAnimationCall("ACTION_UNSTEALTH", "performPostAnimation");
        // move unit to target position
        unit.moveUnitAction(action);
        ACTION_UNSTEALTH.postAnimationUnit = unit;

    };
    this.postAnimationUnit = null;
    this.performPostAnimation = function(action)
    {
        // disable unit commandments for this turn
        var animation = GameAnimationFactory.createAnimation(ACTION_UNSTEALTH.postAnimationUnit.getX(), ACTION_UNSTEALTH.postAnimationUnit.getY());
        if (ACTION_UNSTEALTH.postAnimationUnit.getUnitID() === "SUBMARINE" ||
            ACTION_UNSTEALTH.postAnimationUnit.getUnitID() === "ZCOUNIT_MISSILE_SUB")
        {
            animation.addSprite("undive", -map.getImageSize() / 2, -map.getImageSize() / 2, 0, 2);
            animation.setSound("undive.wav", 1,);
        }
        else
        {
            animation.addSprite("stealth", -map.getImageSize() / 2, -map.getImageSize() / 2, 0, 2);
            animation.setSound("unstealth.wav", 1,);
        }
        ACTION_UNSTEALTH.postAnimationUnit.setHasMoved(true);
        ACTION_UNSTEALTH.postAnimationUnit.setHidden(false);
        ACTION_UNSTEALTH.postAnimationUnit = null;
    }
    this.getDescription = function()
    {
        return qsTr("Unstealths a unit and makes it visible to all enemies.");
    };
}

Constructor.prototype = ACTION;
var ACTION_UNSTEALTH = new Constructor();
