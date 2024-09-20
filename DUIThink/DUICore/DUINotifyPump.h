#pragma once

class DUITHINK_API CDUINotifyPump : public IDuiNotify
{
	DuiDeclare_Message_Map()

public:
	void OnNotify(const DuiNotify &Notify) override;
};
