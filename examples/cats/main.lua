require("EventListener")

--
-- Setup
--

--
SetResolution(GetDisplayWidth(), GetDisplayHeight(), 30);

--
local catTexture = LoadTexture('cat.png')

local cats = {}

for i = 1, 32 do
	local cat = {
		texture = catTexture,
		xPosition = math.random(GetWidth()),
		yPosition = math.random(GetHeight())
	}

	table.insert(cats, cat)
end

--
local font = LoadFont("Arial")

local text = ""

local onceShiftPressed = RegisterEvent
{
	condition = function () return Keyboard.Shift == true end,
	callback = function () print "Keyboard.Shift == true" end,
	once = true
}

local onceShiftReleased = RegisterEvent
{
	condition = function () return Keyboard.Shift == false end,
	callback = function () print "Keyboard.Shift == false" end,
	once = true,
	before = true
}

--
local elapsedTime = 0

--
-- Callbacks
--

function OnDestroy()
	UnregisterEvent(onceShiftReleased)
	UnregisterEvent(onceShiftPressed)
	ReleaseFont(font)
	ReleaseTexture(catTexture)
end

function OnUpdate(deltaTime)
	ProcessEvents()

	for _, cat in pairs(cats) do
		--
	end
	
	elapsedTime = elapsedTime + deltaTime
end

function OnRender(deltaTime)	
	for _, cat in pairs(cats) do
		DrawSprite
		{
			texture = cat.texture,
			xPosition = cat.xPosition + math.sin(elapsedTime * 8) * 16,
			yPosition = cat.yPosition + math.sin(elapsedTime * 4) * 16
		}
	end
	
	DrawText
	{
		text = text,
		font = font,
		size = 128,
		x = 0,
		y = 0,
		color = 0xFF0099FF
	}
end
