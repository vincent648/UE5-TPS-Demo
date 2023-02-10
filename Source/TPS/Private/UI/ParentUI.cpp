// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ParentUI.h"

void UParentUI::EnterNewState(EStackOperation StackOperation)
{
	BP_EnterNewState(StackOperation);
}

void UParentUI::ExitOldState(EStackOperation StackOperation, bool Hide)
{
	BP_ExitOldState(StackOperation, Hide);
}

void UParentUI::Init_Implementation()
{

}

void UParentUI::BP_EnterNewState_Implementation(EStackOperation StackOperation)
{
	switch (StackOperation)
	{
		case Push:
		{
			AddToViewport();
			break;
		}

		case Pop:
		{
		SetVisibility(ESlateVisibility::Visible);
		break;
		}
	}
}

void UParentUI::BP_ExitOldState_Implementation(EStackOperation StackOperation, bool Hide)
{
	switch (StackOperation)
	{
		case Push:
		{
			if (Hide)
			{
				SetVisibility(ESlateVisibility::Collapsed);
				break;
			}
		}

		case Pop:
		{
			RemoveFromParent();
			break;
		}
	}
}