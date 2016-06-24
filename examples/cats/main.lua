require("EventListener")

--
-- Setup
--

--
SetResolution(GetDisplayWidth(), GetDisplayHeight(), 30);

--
local catImage = LoadImage('cat.png')

local cats = {}

for i = 1, 32 do
	local cat = {
		image = catImage,
		xPosition = math.random(GetWidth()),
		yPosition = math.random(GetHeight())
	}

	table.insert(cats, cat)
end

--
local font = LoadFont("Arial")

local text = ""

local eventShiftPressed = RegisterEvent
{
	condition = function () return Keyboard.Shift == true end,
	callback = function () text = "Keyboard.Shift == true" end,
	once = true
}

local eventShiftReleased = RegisterEvent
{
	condition = function () return Keyboard.Shift == false end,
	callback = function () text = "Keyboard.Shift == false" end,
	once = true,
	before = true
}

--
local elapsedTime = 0

--
-- Callbacks
--

function OnDestroy()
	UnregisterEvent(eventShiftReleased)
	UnregisterEvent(eventShiftPressed)
	ReleaseFont(font)
	ReleaseImage(catImage)
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
			image = cat.image,
			x = cat.xPosition + math.sin(elapsedTime * 8) * 16,
			y = cat.yPosition + math.sin(elapsedTime * 4) * 16
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
