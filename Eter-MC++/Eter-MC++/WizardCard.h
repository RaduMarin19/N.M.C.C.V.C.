#pragma once

#include "Card.h"
#include "WizardType.h"

class WizardCard : public Card
{
public:
	WizardCard(const Coordinates& position, CardTexture* texture, WizardType wizard, unsigned short id);

	bool IsUsed()  const;
	void SetUsed(bool used);

	void SetWizard(WizardType wizard, CardTexture* texture);
	WizardType GetWizard() const;

private:
	WizardType m_wizard;
	bool m_used;
};

