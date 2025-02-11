var Constructor = function()
{
    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender, action)
    {
		if (CO_PERK.isActive(co))
		{
			if (typeof map !== 'undefined')
			{
				if (map.getGameRules().getCurrentWeather().getWeatherId() === "WEATHER_SANDSTORM")
                {
					return 20;
				}
			}
		}
        return 0;
    };
	// Perk - Intel
    this.getDescription = function()
    {
        return qsTr("Increases the attack of units by 20% during sandstorm.");
    };
    this.getIcon = function()
    {
        return "sandscorpion";
    };
    this.getName = function()
    {
        return qsTr("Sand Scorpion");
    };    
    this.getGroup = function()
    {
        return qsTr("Weather");
    };
};

Constructor.prototype = CO_PERK;
var CO_PERK_SAND_SCORPION = new Constructor();
