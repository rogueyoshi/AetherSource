local events = {}
local eventCount = 0

function registerEvent (event)
	table.insert(events, event)
	return eventCount + 1
end

function unregisterEvent (event)
	table.remove(events, event)
	eventCount = eventCount - 1
end

function processEvents ()
	for _, event in pairs(events) do
		for _, condition in pairs(event.conditions) do
			if condition() then
				event.triggered = true
			else
				event.triggered = false
				event.triggeredBefore = false
				break
			end
		end
		if event.triggered and not event.triggeredBefore then
			event.callback()
			event.triggeredBefore = true
		end
	end
end
