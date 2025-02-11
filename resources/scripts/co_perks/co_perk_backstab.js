var Constructor = function()
{
    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender, action)
    {
		if (CO_PERK.isActive(co))
		{
			if (attacker.isStatusStealthed())
			{
				return 15;
			}
		}
        return 0;
    };
	// Perk - Intel
    this.getDescription = function()
    {
        return qsTr("Increases the dive/hide attack by 15%.");
    };
    this.getIcon = function()
    {
        return "seamanship";
    };
    this.getName = function()
    {
        return qsTr("Backstab");
    };
    this.getGroup = function()
    {
        return qsTr("Offensive");
    };
};

Constructor.prototype = CO_PERK;
var CO_PERK_BACKSTAB = new Constructor();
