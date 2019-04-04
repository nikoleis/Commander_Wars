var Constructor = function()
{
    // loader for stuff which needs C++ Support
    this.init = function (terrain)
    {
        terrain.setTerrainName(qsTr("Pipeline"));
    };
	this.loadBaseTerrain = function(terrain)
    {
		terrain.loadBaseTerrain("PLAINS");
    };
    this.loadBaseSprite = function(terrain)
    {
        var surroundings = terrain.getSurroundings("PIPELINE,WELD,DESTROYEDWELD,PIPESTATION", false, false, GameEnums.Directions_Direct, true, true);
        if (surroundings === "")
        {
            terrain.loadBaseSprite("pipeline+E+W");
        }
        else
        {
            terrain.loadBaseSprite("pipeline" + surroundings);
        }
    };
    this.getMiniMapIcon = function()
    {
        return "minimap_pipeline";
    };
    this.getTerrainAnimationForeground = function(unit, terrain)
    {
        return "fore_pipeline";
    };
};
Constructor.prototype = TERRAIN;
var PIPELINE = new Constructor();
