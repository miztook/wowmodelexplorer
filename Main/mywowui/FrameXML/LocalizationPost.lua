


function LocalizePost()
	-- Put all locale specific string adjustments here
end

function AbbreviateLargeNumbers(value)
	local strLen = strlen(value);
	local retString = value;
	if ( strLen >= 11 ) then
		retString = string.sub(value, 1, -8)..SECOND_NUMBER_CAP;
	elseif ( strLen >= 9 ) then
		retString = string.sub(value, 1, -9).."."..string.sub(value, -8, -7)..SECOND_NUMBER_CAP;
	elseif ( strLen >= 7 ) then
		retString = string.sub(value, 1, -5)..FIRST_NUMBER_CAP;
	elseif (strLen > 3 ) then
		retString = BreakUpLargeNumbers(value);
	end
	return retString;
end

function PlayerFrame_UpdateLevelTextAnchor(level)
	if ( level >= 100 ) then
		PlayerLevelText:SetPoint("CENTER", PlayerFrameTexture, "CENTER", -62, -15);
	else
		PlayerLevelText:SetPoint("CENTER", PlayerFrameTexture, "CENTER", -61, -15);
	end
end

function TargetFrame_UpdateLevelTextAnchor (self, targetLevel)
	if ( targetLevel >= 100 ) then
		self.levelText:SetPoint("CENTER", 61, -15);
	else
		self.levelText:SetPoint("CENTER", 62, -15);
	end
end

function BossTargetFrame_UpdateLevelTextAnchor (self, targetLevel)
	self.levelText:SetPoint("CENTER", 11, -16);
end