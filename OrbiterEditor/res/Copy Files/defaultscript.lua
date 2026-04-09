local GameObject = {}

function GameObject:Start()
    LogOB("This is called at the beginning of the game.")
end

function GameObject:Update()
    LogOB("This is called every frame.")
end

return GameObject