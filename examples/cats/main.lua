local width = GetDisplayWidth()
local height = GetDisplayHeight()
local fps = 30

SetResolution(width, height, fps);

local catImage = LoadImage("cat.png")

local cats = {}

for i = 1, 32 do
	local cat = {
		image = catImage,
		xPosition = math.random(width),
		yPosition = math.random(height)
	}

	table.insert(cats, cat)
end

local time = 0

function OnDestroy()
	--
end

function OnUpdate(dt)	
	for _, cat in pairs(cats) do
		--
	end
	
	time = time + dt
end

function OnRender(dt)	
	for _, cat in pairs(cats) do
		DrawSprite(cat.image, cat.xPosition + math.sin(time * 8) * 16, cat.yPosition + math.sin(time * 8) * 16)
	end
	
	--drawText ('Hello World', 'Arial', 128, 100, 50, 0xff0099ff)
end
