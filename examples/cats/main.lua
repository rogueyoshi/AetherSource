SetResolution(GetDisplayWidth(), GetDisplayHeight(), 30);

local catImage = LoadImage("cat.png")

local cats = {}

for i = 1, 32 do
	local cat = {
		image = catImage,
		xPosition = math.random(GetWidth()),
		yPosition = math.random(GetHeight())
	}

	table.insert(cats, cat)
end

local time = 0

function OnDestroy()
	--
end

function OnUpdate(deltaTime)
	print(Keyboard.Shift, Keyboard.Control, Keyboard.Alt, Keyboard.Windows)

	for _, cat in pairs(cats) do
		--
	end
	
	time = time + deltaTime
end

function OnRender(deltaTime)	
	for _, cat in pairs(cats) do
		DrawSprite(cat.image, cat.xPosition + math.sin(time * 8) * 16, cat.yPosition + math.sin(time * 4) * 16)
	end
	
	DrawText('Hello World', 'Arial', 128, 100, 50, 0xFF0099FF)
end
