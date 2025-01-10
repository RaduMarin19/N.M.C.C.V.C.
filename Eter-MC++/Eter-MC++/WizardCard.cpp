#include "WizardCard.h"

WizardCard::WizardCard(const Coordinates& position, CardTexture* texture, WizardType wizard, unsigned short id) : Card{ position, texture, id }, m_wizard{ wizard }, m_used{ false } {}

bool WizardCard::IsUsed() const
{
	return m_used;
}

void WizardCard::SetUsed(bool used)
{
	m_used = used;
}

void WizardCard::SetWizard(WizardType wizard, CardTexture* texture)
{
	m_wizard = wizard;
	m_texture = texture;
}

WizardType WizardCard::GetWizard() const
{
	return m_wizard;
}

