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

function OnDestroy()
	print("OnDestroy");
end

function OnUpdate(dt)
	--print("OnUpdate", dt);
	
	for _, cat in pairs(cats) do
		--
	end
end

function OnRender()
	--print("OnRender");
	
	for _, cat in pairs(cats) do
		--print("DrawSprite", cat.image, cat.xPosition, cat.yPosition)
		DrawSprite(cat.image, cat.xPosition, cat.yPosition)
	end
	
	--drawText ('Hello World', 'Arial', 128, 100, 50, 0xff0099ff)
end
