#include "menu.h"
#include "menufunctions.h"
#include "global.h"
#include "commonfunctions.h"
#include "draw.h"
#include "codes.h"
#include "mod.h"
#include "memcard.h"
#include "memorywatch.h"

#include <gc/card.h>
#include <ttyd/swdrv.h>
#include <ttyd/evt_yuugijou.h>
#include <ttyd/mario_pouch.h>
#include <ttyd/win_party.h>
#include <ttyd/mario_party.h>
#include <ttyd/win_main.h>

#include <cstdio>

namespace mod {

void menuCheckButton()
{
	// Only run if a button was pressed
	uint32_t CurrentButton 					= checkButtonSingleFrame();
	if (CurrentButton == 0)
	{
		return;
	}
	
	uint32_t tempCurrentMenu 				= CurrentMenu;
	uint32_t tempCurrentMenuOption 			= CurrentMenuOption;
	// uint32_t tempSecondaryMenuOption 	= SecondaryMenuOption;
	// uint32_t tempTotalMenuColumns 		= Menu[tempCurrentMenu].TotalMenuColumns;
	// uint32_t tempColumnSplitAmount 		= Menu[tempCurrentMenu].ColumnSplitAmount;
	// uint32_t tempTotalMenuOptions 		= Menu[tempCurrentMenu].TotalMenuOptions;
	uint32_t tempSelectedOption 			= SelectedOption;
	uint32_t tempCurrentPage 				= CurrentPage;
	uint32_t tempMenuSelectionStates 		= MenuSelectionStates;
	uint32_t tempMenuSelectedOption 		= MenuSelectedOption;
	// uint32_t tempMenuSecondaryValue 		= MenuSecondaryValue;
	
	switch (tempCurrentMenu)
	{
		case ROOT:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case BATTLES:
						{
							// Make sure the player is currently in a battle
							if (getBattleWorkPointer())
							{
								// Currently in a battle, so enter the next menu
								enterNextMenu(BATTLES, tempCurrentMenuOption);
								resetMenu();
							}
							else
							{
								FunctionReturnCode = NOT_IN_BATTLE;
								Timer = secondsToFrames(3);
							}
							break;
						}
						default:
						{
							// Enter the next menu
							enterNextMenu(CurrentMenuOptionCheck, tempCurrentMenuOption);
							resetMenu();
							break;
						}
					}
					break;
				}
				case B:
				{
					// Close the menu
					closeMenu();
					return;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case INVENTORY:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					MenuSelectedOption = tempCurrentMenuOption;
					enterNextMenu(INVENTORY_MAIN, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					CurrentMenuOption = enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case INVENTORY_MAIN:
		{
			uint32_t MaxOptionsPerPage = 20;
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						case DUPLICATE:
						case DELETE:
						{
							adjustMenuSelectionInventory(CurrentButton);
							break;
						}
						case CHANGE_BY_ID:
						case CHANGE_BY_ICON:
						{
							if (tempMenuSelectionStates == 0)
							{
								adjustMenuSelectionInventory(CurrentButton);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case R:
				{
					if (checkForItemsOnNextPage(tempCurrentPage))
					{
						// Go to the next page
						if (tempMenuSelectionStates == 0)
						{
							tempCurrentPage++;
							CurrentPage = tempCurrentPage;
							
							if ((tempSelectedOption >= DUPLICATE) && 
								(tempSelectedOption <= DELETE))
							{
								CurrentMenuOption = tempCurrentPage * MaxOptionsPerPage;
							}
						}
					}
					break;
				}
				case L:
				{
					if (tempCurrentPage > 0)
					{
						// Go to the previous page
						if (tempMenuSelectionStates == 0)
						{
							tempCurrentPage--;
							CurrentPage = tempCurrentPage;
							
							if ((tempSelectedOption >= DUPLICATE) && 
								(tempSelectedOption <= DELETE))
							{
								CurrentMenuOption = tempCurrentPage * MaxOptionsPerPage;
							}
						}
					}
					break;
				}
				case A:
				{
					correctInventoryCurrentMenuOptionAndPage(MaxOptionsPerPage);
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case ADD_BY_ID:
								{
									if (getFreeSlotPointer())
									{
										SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempMenuSelectedOption) - 1;
										Timer 					= 0;
										SelectedOption 			= CurrentMenuOptionCheck;
									}
									else
									{
										// Inventory is currently full
										FunctionReturnCode 		= INVENTORY_FULL;
										Timer 					= secondsToFrames(3);
										SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								case ADD_BY_ICON:
								{
									if (getFreeSlotPointer())
									{
										SecondaryMenuOption 	= 0;
										Timer 					= 0;
										SelectedOption 			= CurrentMenuOptionCheck;
									}
									else
									{
										// Inventory is currently full
										FunctionReturnCode 		= INVENTORY_FULL;
										Timer 					= secondsToFrames(3);
										SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								case DUPLICATE:
								{
									if (getTotalItems() > 0)
									{
										if (getFreeSlotPointer())
										{
											CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
											Timer 				= 0;
											SelectedOption 		= CurrentMenuOptionCheck;
										}
										else
										{
											// Inventory is currently full
											FunctionReturnCode 	= INVENTORY_FULL;
											Timer 				= secondsToFrames(3);
											SelectedOption 		= CurrentMenuOptionCheck;
										}
									}
									else
									{
										// Inventory is currently empty
										FunctionReturnCode 		= INVENTORY_EMPTY;
										Timer 					= secondsToFrames(3);
										SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								case CHANGE_BY_ID:
								case CHANGE_BY_ICON:
								case DELETE:
								{
									if (getTotalItems() > 0)
									{
										CurrentMenuOption 		= tempCurrentPage * MaxOptionsPerPage;
										Timer 					= 0;
										SelectedOption 			= CurrentMenuOptionCheck;
									}
									else
									{
										// Inventory is currently empty
										FunctionReturnCode 		= INVENTORY_EMPTY;
										Timer 					= secondsToFrames(3);
										SelectedOption 			= CurrentMenuOptionCheck;
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						case ADD_BY_ID:
						case ADD_BY_ICON:
						{
							if (getFreeSlotPointer())
							{
								Timer = 0;
							}
							else
							{
								// Inventory is currently full
								FunctionReturnCode = INVENTORY_FULL;
								Timer = secondsToFrames(3);
							}
							break;
						}
						case DUPLICATE:
						{
							if (getTotalItems() > 0)
							{
								Timer = 0;
								
								void *tempAddress = getFreeSlotPointer();
								if (tempAddress)
								{
									duplicateCurrentItem(tempAddress);
									
									// Recheck the inventory
									if (!getFreeSlotPointer())
									{
										// No more free spaces, so leave the duplicate menu
										closeSecondaryMenu();
									}
								}
								else
								{
									// Inventory is currently full
									FunctionReturnCode = INVENTORY_FULL;
									Timer = secondsToFrames(3);
								}
							}
							else
							{
								// Inventory is currently empty
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer 				= secondsToFrames(3);
								SelectedOption 		= tempCurrentMenuOption;
							}
							break;
						}
						case CHANGE_BY_ID:
						{
							if (getTotalItems() > 0)
							{
								uint32_t Address_and_Size[2];
								uint32_t *tempArray	= getPouchAddressAndSize(Address_and_Size);
								
								if (tempArray)
								{
									uint32_t tempAddress = tempArray[0];
									uint32_t CurrentItemAddress = tempAddress + (tempCurrentMenuOption * 0x2);
									int16_t CurrentItem = *reinterpret_cast<int16_t *>(CurrentItemAddress);
									MenuSecondaryValue = CurrentItem;
								}
								else
								{
									MenuSecondaryValue = 0;
								}
								
								Timer = 0;
								if (tempMenuSelectionStates == 0)
								{
									MenuSelectionStates = tempSelectedOption;
									SecondaryMenuOption = getHighestAdjustableValueDigit(tempMenuSelectedOption) - 1;
								}
							}
							else
							{
								// Inventory is currently empty
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer 				= secondsToFrames(3);
								SelectedOption 		= tempCurrentMenuOption;
							}
							break;
						}
						case CHANGE_BY_ICON:
						{
							if (getTotalItems() > 0)
							{
								uint32_t Address_and_Size[2];
								uint32_t *tempArray	= getPouchAddressAndSize(Address_and_Size);
								
								if (tempArray)
								{
									uint32_t tempAddress = tempArray[0];
									uint32_t CurrentItemAddress = tempAddress + (tempCurrentMenuOption * 0x2);
									int16_t CurrentItem = *reinterpret_cast<int16_t *>(CurrentItemAddress);
									
									if (CurrentItem != 0)
									{
										int32_t UpperAndLowerBounds[2];
										getUpperAndLowerBounds(UpperAndLowerBounds, tempMenuSelectedOption);
										
										int32_t LowerBound = UpperAndLowerBounds[0];
										SecondaryMenuOption = CurrentItem - LowerBound;
									}
									else
									{
										SecondaryMenuOption = 0;
									}
								}
								else
								{
									SecondaryMenuOption = 0;
								}
								
								Timer = 0;
								if (tempMenuSelectionStates == 0)
								{
									MenuSelectionStates = tempSelectedOption;
								}
							}
							else
							{
								// Inventory is currently empty
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer 				= secondsToFrames(3);
								SelectedOption 		= tempCurrentMenuOption;
							}
							break;
						}
						case DELETE:
						{
							if (getTotalItems() > 0)
							{
								Timer = 0;
								deleteItem();
								
								// Recheck the inventory
								if (getTotalItems() <= 0)
								{
									// Inventory is currently empty, so leave the delete menu
									closeSecondaryMenu();
								}
							}
							else
							{
								// Inventory is currently empty
								FunctionReturnCode 	= INVENTORY_EMPTY;
								Timer 				= secondsToFrames(3);
								SelectedOption 		= tempCurrentMenuOption;
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							MenuSelectedOption = 0;
							resetMenu();
							CurrentMenuOption = enterPreviousMenu();
							break;
						}
						case CHANGE_BY_ID:
						case CHANGE_BY_ICON:
						{
							if (tempMenuSelectionStates == 0)
							{
								resetAndCloseSecondaryMenu();
							}
							break;
						}
						default:
						{
							resetAndCloseSecondaryMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					default_DPAD_Actions(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					uint32_t MenuToEnter;
					switch (tempCurrentMenuOption)
					{
						case CHANGE_SEQUENCE:
						{
							MenuToEnter = CHEATS_CHANGE_SEQUENCE;
							break;
						}
						case WALK_THROUGH_WALLS:
						case SAVE_COORDINATES:
						case LOAD_COORDINATES:
						case SPAWN_ITEM:
						case SAVE_ANYWHERE:
						case TEXT_STORAGE:
						case TIME_STOP_TEXT_STORAGE:
						case SPEED_UP_MARIO:
						case DISABLE_BATTLES:
						case AUTO_ACTION_COMMANDS:
						case INFINITE_ITEM_USAGE:
						case RELOAD_ROOM:
						case LEVITATE:
						{
							MenuToEnter = CHEATS_STANDARD;
							break;
						}
						case LOCK_MARIO_HP_TO_MAX:
						case RUN_FROM_BATTLES:
						case DISABLE_MENU_SOUNDS:
						case BOBBERY_EARLY:
						{
							MenuToEnter = CHEATS_NO_BUTTON_COMBO;
							break;
						}
						case FORCE_ITEM_DROP:
						{
							MenuToEnter = CHEATS_NPC_FORCE_DROP;
							break;
						}
						case MANAGE_FLAGS:
						{
							MenuToEnter = CHEATS_MANAGE_FLAGS;
							break;
						}
						case CLEAR_AREA_FLAGS:
						default:
						{
							MenuToEnter = CHEATS_CLEAR_AREA_FLAGS;
							break;
						}
					}
					
					MenuSelectedOption = tempCurrentMenuOption;
					enterNextMenu(MenuToEnter, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					CurrentMenuOption = enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_CHANGE_SEQUENCE:
		{
			switch (CurrentButton)
			{
				case A:
				{
					if (tempMenuSelectionStates == 0)
					{
						MenuSecondaryValue 	= static_cast<int32_t>(getSequencePosition());
						MenuSelectionStates = CHANGE_SEQUENCE_VALUE;
						SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectionStates != CHANGE_SEQUENCE_VALUE)
					{
						// Go back to the previous menu
						MenuSelectedOption = 0;
						CurrentMenuOption = enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_STANDARD:
		case CHEATS_NO_BUTTON_COMBO:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempCurrentMenu == CHEATS_STANDARD)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case TURN_ON_OR_OFF:
								{
									// Flip the bool for the current cheat
									bool CheatActive = Cheat[tempMenuSelectedOption].Active;
									Cheat[tempMenuSelectedOption].Active = !CheatActive;
									break;
								}
								case CHANGE_BUTTON_COMBO:
								{
									ChangingCheatButtonCombo 	= true;
									Timer 						= secondsToFrames(3);
									SelectedOption 				= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							// Currently changing a cheat button combo, so do nothing
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case CHANGE_BUTTON_COMBO:
						{
							// The work for this is done in drawChangeButtonCombo
							break;
						}
						default:
						{
							// Go back to the previous menu
							MenuSelectedOption = 0;
							CurrentMenuOption = enterPreviousMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_NPC_FORCE_DROP:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempMenuSelectionStates == 0)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case ITEM_DROP_TURN_ON_OR_OFF:
						{
							// Flip the bool for the current cheat
							bool CheatActive = Cheat[tempMenuSelectedOption].Active;
							Cheat[tempMenuSelectedOption].Active = !CheatActive;
							break;
						}
						case ITEM_DROP_CHANGE_BY_ID:
						{
							if (tempMenuSelectionStates == 0)
							{
								MenuSelectionStates = CurrentMenuOptionCheck;
								SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectionStates != ITEM_DROP_CHANGE_BY_ID)
					{
						// Go back to the previous menu
						MenuSelectedOption = 0;
						CurrentMenuOption = enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_MANAGE_FLAGS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Clear the current values set for managing flags
					clearMemory(&ManageFlags, sizeof(ManageFlags));
					
					// Enter the next menu
					MenuSelectionStates = tempCurrentMenuOption + 1;
					enterNextMenu(CHEATS_MANAGE_FLAGS_MAIN, tempCurrentMenuOption);
					CurrentMenuOption = 0;
					break;
				}
				case B:
				{
					// Go back to the previous menu
					MenuSelectedOption = 0;
					CurrentMenuOption = enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_MANAGE_FLAGS_MAIN:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempSelectedOption == 0)
					{
						adjustCheatsManageFlagsMainMenu(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t FlagToSet = ManageFlags.FlagToSet;
							uint32_t ValueToSet = static_cast<uint32_t>(ManageFlags.ValueToSet);
							
							switch (tempMenuSelectionStates)
							{
								case SET_GSW:
								case SET_GW:
								case SET_LSW:
								{
									switch (tempCurrentMenuOption)
									{
										case CHANGE_GLOBAL_WORD:
										{
											MenuSecondaryValue 		= FlagToSet;
											SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											SelectedOption 			= tempCurrentMenuOption + 1;
											break;
										}
										case CHANGE_GLOBAL_WORD_VALUE:
										{
											MenuSecondaryValue 		= getGlobalFlagValue(tempMenuSelectionStates, FlagToSet);
											SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											SelectedOption 			= tempCurrentMenuOption + 1;
											break;
										}
										case SET_GLOBAL_WORD_VALUE:
										{
											switch (tempMenuSelectionStates)
											{
												case SET_GSW:
												{
													ttyd::swdrv::swByteSet(FlagToSet, ValueToSet);
													break;
												}
												case SET_GW:
												{
													setGW(FlagToSet, ValueToSet);
													break;
												}
												case SET_LSW:
												{
													ttyd::swdrv::_swByteSet(FlagToSet, ValueToSet);
													break;
												}
												default:
												{
													break;
												}
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								case SET_GSWF:
								case SET_GF:
								case SET_LSWF:
								{
									switch (tempCurrentMenuOption)
									{
										case CHANGE_GLOBAL_FLAG:
										{
											MenuSecondaryValue 		= FlagToSet;
											SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											SelectedOption 			= tempCurrentMenuOption + 1;
											break;
										}
										case SET_GLOBAL_FLAG:
										{
											switch (tempMenuSelectionStates)
											{
												case SET_GSWF:
												{
													if (ttyd::swdrv::swGet(FlagToSet))
													{
														ttyd::swdrv::swClear(FlagToSet);
													}
													else
													{
														ttyd::swdrv::swSet(FlagToSet);
													}
													break;
												}
												case SET_GF:
												{
													setGF(FlagToSet); // setGF automatically toggles the value
													break;
												}
												case SET_LSWF:
												{
													if (ttyd::swdrv::_swGet(FlagToSet))
													{
														ttyd::swdrv::_swClear(FlagToSet);
													}
													else
													{
														ttyd::swdrv::_swSet(FlagToSet);
													}
													break;
												}
												default:
												{
													break;
												}
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempSelectedOption == 0)
					{
						// Go back to the previous menu
						MenuSelectionStates = 0;
						CurrentMenuOption = enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS_CLEAR_AREA_FLAGS:
		{
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				{
					switch (tempSelectedOption)
					{
						case SELECT_AREA:
						{
							adjustCheatClearAreaFlagSelection(CurrentButton);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						case SELECT_AREA:
						{
							adjustCheatClearAreaFlagSelection(CurrentButton);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case SELECT_AREA:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									SecondaryMenuOption 	= 0;
									SelectedOption 			= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									MenuSecondaryValue 		= SecondaryMenuOption;
									SelectedOption 			= 0;
									break;
								}
							}
							break;
						}
						case CLEAR_FLAGS:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									SecondaryMenuOption 	= 1;
									SelectedOption 			= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									switch (SecondaryMenuOption)
									{
										case 0:
										{
											// Selected yes
											ClearAreaFlags.CurrentOption = MenuSecondaryValue;
											ClearAreaFlags.FlagsShouldBeCleared = true;
											SelectedOption = 0;
											break;
										}
										default:
										{
											// Selected no
											SelectedOption = 0;
											break;
										}
									}
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case SELECT_AREA:
						case CLEAR_FLAGS:
						{
							SelectedOption = 0;
							break;
						}
						default:
						{
							// Go back to the previous menu
							MenuSelectedOption = 0;
							CurrentMenuOption = enterPreviousMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case STATS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					MenuSelectedOption = 0;
					enterNextMenu(STATS_MARIO + tempCurrentMenuOption, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					CurrentMenuOption = enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case STATS_MARIO:
		{
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				case DPADDOWN:
				case DPADUP:
				{
					adjustMarioStatsSelection(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							MenuSelectedOption = CurrentMenuOptionCheck;
							MenuSelectionStates = CurrentMenuOptionCheck;
							
							switch (CurrentMenuOptionCheck)
							{
								case SPECIAL_MOVES:
								{
									SecondaryMenuOption = 0;
									break;
								}
								default:
								{
									SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
								
									uint32_t offset = getMarioStatsValueOffset(CurrentMenuOptionCheck);
									if (offset == 0)
									{
										break;
									}
									
									// Set the current value in the display to the current value of the address being changed
									if ((CurrentMenuOptionCheck >= PIANTAS_STORED) && 
										(CurrentMenuOptionCheck <= CURRENT_PIANTAS))
									{
										uint32_t PiantaParlorPtr = reinterpret_cast<uint32_t>(
											ttyd::evt_yuugijou::yuugijouWorkPointer);
										
										MenuSecondaryValue = *reinterpret_cast<int32_t *>(PiantaParlorPtr + offset);
									}
									else
									{
										uint32_t PouchPtr = reinterpret_cast<uint32_t>(
											ttyd::mario_pouch::pouchGetPtr());
										
										MenuSecondaryValue = *reinterpret_cast<int16_t *>(PouchPtr + offset);
									}
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectedOption == 0)
					{
						// Go back to the previous menu
						resetMenu();
						CurrentMenuOption = enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case STATS_PARTNERS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						default:
						{
							adjustPartnerStatsSelection(CurrentButton);
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							CurrentMenuOption = 0;
							SelectedOption = tempCurrentMenuOption + 1;
							break;
						}
						default:
						{
							switch (tempMenuSelectedOption)
							{
								case 0:
								{
									uint32_t PartnerEnabledAddress = reinterpret_cast<uint32_t>(getPartnerEnabledAddress());
									
									uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
									switch (CurrentMenuOptionCheck)
									{
										case PARTNER_HP:
										{
											MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0x6);
											
											MenuSelectedOption = CurrentMenuOptionCheck;
											MenuSelectionStates = CurrentMenuOptionCheck;
											SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case PARTNER_MAX_HP:
										{
											MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0x2);
											
											MenuSelectedOption = CurrentMenuOptionCheck;
											MenuSelectionStates = CurrentMenuOptionCheck;
											SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case PARTNER_RANK:
										{
											MenuSecondaryValue = *reinterpret_cast<int16_t *>(
												PartnerEnabledAddress + 0xA);
											
											MenuSelectedOption = CurrentMenuOptionCheck;
											MenuSelectionStates = CurrentMenuOptionCheck;
											SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
											break;
										}
										case TOGGLE:
										{
											bool *AddressToChange = reinterpret_cast<bool *>(
												PartnerEnabledAddress + 1);
											
											*AddressToChange = !*AddressToChange;
											
											// Reset the partner menu if the pause menu is open
											uint32_t SystemLevel = getSystemLevel();
											if (SystemLevel == 15)
											{
												void *PauseMenuPointer = ttyd::win_main::winGetPtr();
												ttyd::win_party::winPartyExit(PauseMenuPointer);
												ttyd::win_party::winPartyInit(PauseMenuPointer);
											}
											break;
										}
										case TOGGLE + 1:
										{
											// Check if currently on Yoshi
											const uint32_t YoshiPartner = 4;
											uint32_t CurrentPartner = getSelectedOptionPartnerValue();
											if (CurrentPartner == YoshiPartner)
											{
												MenuSelectedOption = STATS_PARTNER_DISPLAY_YOSHI_COLORS;
												uint32_t CurrentColorId = getCurrentYoshiColorId();
												
												// Make sure the current color is valid
												const uint32_t ColorIdWhite = 6;
												if (CurrentColorId <= ColorIdWhite)
												{
													SecondaryMenuOption = CurrentColorId;
												}
												else
												{
													SecondaryMenuOption = 0;
												}
											}
											else
											{
												partnerMenuRemoveOrBringOut(reinterpret_cast<void *>(PartnerEnabledAddress));
											}
											break;
										}
										case TOGGLE + 2:
										{
											// This option should only be available for Yoshi
											const uint32_t YoshiPartner = 4;
											uint32_t CurrentPartner = getSelectedOptionPartnerValue();
											if (CurrentPartner == YoshiPartner)
											{
												partnerMenuRemoveOrBringOut(reinterpret_cast<void *>(PartnerEnabledAddress));
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempMenuSelectedOption == 0)
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								// Go back to the previous menu
								resetMenu();
								CurrentMenuOption = enterPreviousMenu();
								break;
							}
							default:
							{
								closeSecondaryMenu();
								break;
							}
						}
					}
					break;
				}
			}
			break;
		}
		case STATS_FOLLOWERS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempSelectedOption == 0)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case BRING_OUT_FOLLOWER:
								{
									// Make sure a file is loaded
									if (checkIfInGame())
									{
										SecondaryMenuOption = 0;
										Timer 				= 0;
										SelectedOption 		= CurrentMenuOptionCheck;
									}
									else
									{
										FunctionReturnCode 	= NOT_IN_GAME;
										Timer 				= secondsToFrames(3);
									}
									break;
								}
								case REMOVE_FOLLOWER:
								{
									removeFollowerFromOverworld();
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							// Make sure a file is loaded again
							if (checkIfInGame())
							{
								Timer = 0;
							}
							else
							{
								FunctionReturnCode 	= NOT_IN_GAME;
								Timer 				= secondsToFrames(3);
							}
							break;
						}
					}
					break;
				}
				case B:
				{
					if (tempSelectedOption == 0)
					{
						// Go back to the previous menu
						resetMenu();
						CurrentMenuOption = enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case SETTINGS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					Timer = 0;
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
					switch (CurrentMenuOptionCheck)
					{
						case LOAD_SETTINGS:
						{
							int32_t ReturnCode = loadSettings(MenuSettings.SettingsFileName);
							
							switch (ReturnCode)
							{
								case CARD_RESULT_READY:
								{
									MenuSettings.ReturnCode = LOAD_SUCCESSFUL;
									Timer = secondsToFrames(3);
									break;
								}
								case CARD_RESULT_NOFILE:
								{
									MenuSettings.ReturnCode = LOAD_FAILED_NO_FILE;
									Timer = secondsToFrames(3);
									break;
								}
								default:
								{
									MenuSettings.ReturnCode = LOAD_FAILED;
									Timer = secondsToFrames(3);
									break;
								}
							}
							break;
						}
						case SAVE_SETTINGS:
						{
							int32_t ReturnCode = writeSettings(MenuSettings.SettingsDescription, 
								MenuSettings.SettingsFileName, MenuSettings.RelFileName);
							
							switch (ReturnCode)
							{
								case CARD_RESULT_READY:
								{
									MenuSettings.ReturnCode = SAVE_SUCCESSFUL;
									Timer = secondsToFrames(3);
									break;
								}
								default:
								{
									MenuSettings.ReturnCode = SAVE_FAILED;
									Timer = secondsToFrames(3);
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					// Go back to the root
					resetMenu();
					CurrentMenuOption = enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY:
		{
			uint32_t MaxOptionsPerPage = 10;
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						default:
						{
							adjustMemoryWatchSelection(CurrentButton);
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case ADD_WATCH:
								{
									int32_t EmptyWatchSlot = getEmptyWatchSlot();
									if (EmptyWatchSlot >= 0)
									{
										addMemoryWatch(EmptyWatchSlot);
									}
									else
									{
										// There are no more free slots
										FunctionReturnCode = NO_SLOTS_LEFT;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case DUPLICATE_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										int32_t EmptyWatchSlot = getEmptyWatchSlot();
										if (EmptyWatchSlot >= 0)
										{
											CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
											SelectedOption 		= CurrentMenuOptionCheck;
										}
										else
										{
											// There are no more free slots
											FunctionReturnCode = NO_SLOTS_LEFT;
											Timer = secondsToFrames(3);
										}
									}
									else
									{
										// All slots are empty
										FunctionReturnCode = ALL_SLOTS_EMPTY;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case MODIFY_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
										SelectedOption 		= CurrentMenuOptionCheck;
									}
									else
									{
										// All slots are empty
										FunctionReturnCode = ALL_SLOTS_EMPTY;
										Timer = secondsToFrames(3);
									}
									break;
								}
								case DELETE_WATCH:
								{
									uint32_t tempAddress = MemoryWatch[0].Address;
									if (tempAddress)
									{
										CurrentMenuOption 	= tempCurrentPage * MaxOptionsPerPage;
										SelectedOption 		= CurrentMenuOptionCheck;
									}
									else
									{
										// All slots are empty
										FunctionReturnCode = ALL_SLOTS_EMPTY;
										Timer = secondsToFrames(3);
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						case DUPLICATE_WATCH:
						{
							uint32_t tempAddress = MemoryWatch[0].Address;
							if (tempAddress)
							{
								int32_t EmptyWatchSlot = getEmptyWatchSlot();
								if (EmptyWatchSlot >= 0)
								{
									duplicateWatch(static_cast<int32_t>(tempCurrentMenuOption), EmptyWatchSlot);
									
									// Recheck to see if there are any empty slots left
									if (getEmptyWatchSlot() < 0)
									{
										// There are no more free slots
										closeSecondaryMenu();
									}
								}
								else
								{
									// There are no more free slots
									closeSecondaryMenu();
									FunctionReturnCode = NO_SLOTS_LEFT;
									Timer = secondsToFrames(3);
								}
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								FunctionReturnCode = NO_SLOTS_LEFT;
								Timer = secondsToFrames(3);
							}
							break;
						}
						case MODIFY_WATCH:
						{
							uint32_t tempAddress = MemoryWatch[0].Address;
							if (tempAddress)
							{
								// Enter the next menu
								MenuSelectedOption = tempCurrentMenuOption;
								enterNextMenu(MEMORY_MODIFY, tempSelectedOption - 1);
								resetMenu();
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								FunctionReturnCode = NO_SLOTS_LEFT;
								Timer = secondsToFrames(3);
							}
							break;
						}
						case DELETE_WATCH:
						{
							uint32_t tempAddress = MemoryWatch[0].Address;
							if (tempAddress)
							{
								deleteWatch(static_cast<int32_t>(tempCurrentMenuOption));
							}
							else
							{
								// All slots are empty
								closeSecondaryMenu();
								FunctionReturnCode = NO_SLOTS_LEFT;
								Timer = secondsToFrames(3);
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							resetMenu();
							CurrentMenuOption = enterPreviousMenu();
							break;
						}
						default:
						{
							closeSecondaryMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_MODIFY:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					if (tempSelectedOption == 0)
					{
						adjustMenuNoPageEdit(CurrentButton);
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case CHANGE_ADDRESS:
								{
									// Enter the next menu
									enterNextMenu(MEMORY_CHANGE_ADDRESS, tempCurrentMenuOption);
									resetMenu();
									break;
								}
								case CHANGE_TYPE:
								{
									SecondaryMenuOption = MemoryWatch[tempMenuSelectedOption].Type;
									SelectedOption = CurrentMenuOptionCheck;
									break;
								}
								case SHOW_AS_HEX:
								{
									// Only change for types that can be displayed in hex
									switch (MemoryWatch[tempMenuSelectedOption].Type)
									{
										case string:
										case time:
										{
											break;
										}
										default:
										{
											bool ShowAsHex = MemoryWatch[tempMenuSelectedOption].ShowAsHex;
											MemoryWatch[tempMenuSelectedOption].ShowAsHex = !ShowAsHex;
											break;
										}
									}
									break;
								}
								case CHANGE_WATCH_POSITION:
								{
									HideMenu = true;
									MemoryWatchPosition.PosX = MemoryWatch[tempMenuSelectedOption].PosX;
									MemoryWatchPosition.PosY = MemoryWatch[tempMenuSelectedOption].PosY;
									SelectedOption = CurrentMenuOptionCheck;
									break;
								}
								case DISPLAY_OUT_OF_MENU:
								{
									bool DisplayOutOfMenu = MemoryWatch[tempMenuSelectedOption].Display;
									MemoryWatch[tempMenuSelectedOption].Display = !DisplayOutOfMenu;
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the previous menu
							MenuSelectedOption = 0;
							resetMenu();
							CurrentMenuOption = enterPreviousMenu();
							break;
						}
						default:
						{
							closeSecondaryMenu();
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_CHANGE_ADDRESS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempMenuSelectionStates)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									adjustMenuNoPageEdit(CurrentButton);
									break;
								}
								default:
								{
									adjustMemoryChangeAddressOrPointerSelection(CurrentButton);
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempMenuSelectionStates)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
									switch (CurrentMenuOptionCheck)
									{
										case CHANGE_ADDRESS_OR_POINTERS:
										{
											CurrentMenuOption 	= 0;
											SelectedOption 		= CurrentMenuOptionCheck;
											break;
										}
										case ADD_POINTER_LEVEL:
										{
											// Only add a pointer level if not already at the max
											uint32_t AddressOffsetTotal = MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount;
											if (AddressOffsetTotal < 10)
											{
												MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount = AddressOffsetTotal + 1;
												MemoryWatch[tempMenuSelectedOption].AddressOffset[AddressOffsetTotal] = 0;
											}
											break;
										}
										case REMOVE_POINTER_LEVEL:
										{
											// Only remove a pointer level if there is at least one left
											uint32_t AddressOffsetTotal = MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount;
											if (AddressOffsetTotal > 0)
											{
												MemoryWatch[tempMenuSelectedOption].AddressOffsetAmount = AddressOffsetTotal - 1;
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								case CHANGE_ADDRESS_OR_POINTERS:
								{
									MenuSelectionStates = tempCurrentMenuOption + 1;
									SecondaryMenuOption = 7;
									
									switch (tempCurrentMenuOption)
									{
										case 0:
										{
											MemoryWatchSecondaryValue = MemoryWatch[tempMenuSelectedOption].Address;
											break;
										}
										default:
										{
											MemoryWatchSecondaryValue = MemoryWatch[tempMenuSelectedOption].AddressOffset[tempCurrentMenuOption - 1];
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempMenuSelectionStates)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									// Go back to the previous menu
									resetMenu();
									CurrentMenuOption = enterPreviousMenu();
									break;
								}
								default:
								{
									closeSecondaryMenu();
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case BATTLES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				switch (CurrentButton)
				{
					case DPADDOWN:
					case DPADUP:
					{
						adjustBattlesActorSelection(CurrentButton);
						break;
					}
					case A:
					{
						// Make sure the current slot isn't empty
						uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
						void *CurrentActorPointer = getActorPointer(CurrentMenuOptionCheck); // Add 1 to skip System
						
						if (CurrentActorPointer)
						{
							// Go to the next menu
							MenuSelectedOption = CurrentMenuOptionCheck; // Add 1 to skip System
							enterNextMenu(BATTLES_CURRENT_ACTOR, tempCurrentMenuOption);
							resetMenu();
						}
						break;
					}
					case B:
					{
						// Go back to the previous menu
						resetMenu();
						CurrentMenuOption = enterPreviousMenu();
						break;
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				switch (CurrentButton)
				{
					case DPADDOWN:
					case DPADUP:
					{
						if (tempSelectedOption == 0)
						{
							adjustMenuNoPageEdit(CurrentButton);
						}
						break;
					}
					case A:
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								uint32_t ActorAddress = reinterpret_cast<uint32_t>(
									getActorPointer(tempMenuSelectedOption));
								
								uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
								switch (CurrentMenuOptionCheck)
								{
									case CLEAR_HELD_ITEM:
									{
										if (ActorAddress == 0)
										{
											break;
										}
										
										#ifdef TTYD_US
										uint32_t HeldItemOffset = 0x308;
										uint32_t BadgeFlagOffsetStart = 0x2E0;
										#elif defined TTYD_JP
										uint32_t HeldItemOffset = 0x304;
										uint32_t BadgeFlagOffsetStart = 0x2DC;
										#elif defined TTYD_EU
										uint32_t HeldItemOffset = 0x30C;
										uint32_t BadgeFlagOffsetStart = 0x2E4;
										#endif
										
										// Clear the held item
										*reinterpret_cast<int32_t *>(ActorAddress + HeldItemOffset) = 0;
										
										// Do not clear the equipped badges for Mario or the partners
										uint32_t CurrentActorId = *reinterpret_cast<uint32_t *>(ActorAddress + 0x8);
										const uint32_t MarioId 	= 222;
										const uint32_t MowzId 	= 230;
										
										if ((CurrentActorId >= MarioId) && 
											(CurrentActorId <= MowzId))
										{
											break;
										}
										
										// Clear all of the currently equipped badges
										clearMemory(reinterpret_cast<void *>(ActorAddress + BadgeFlagOffsetStart), 0x28);
										break;
									}
									case CHANGE_ACTOR_STATUSES:
									{
										// Go to the next menu
										enterNextMenu(BATTLES_STATUSES, tempCurrentMenuOption);
										resetMenu();
										break;
									}
									default:
									{
										if (ActorAddress == 0)
										{
											break;
										}
										
										MenuSelectionStates 	= CurrentMenuOptionCheck;
										SecondaryMenuOption 	= getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
										SelectedOption 			= CurrentMenuOptionCheck;
										
										switch (CurrentMenuOptionCheck)
										{
											case CHANGE_ACTOR_HP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x10C);
												break;
											}
											case CHANGE_ACTOR_MAX_HP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x108);
												break;
											}
											case CHANGE_ACTOR_FP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x112);
												break;
											}
											case CHANGE_ACTOR_MAX_FP:
											{
												MenuSecondaryValue = *reinterpret_cast<int16_t *>(
													ActorAddress + 0x10E);
												break;
											}
											case CHANGE_HELD_ITEM:
											{
												#ifdef TTYD_US
												uint32_t offset = 0x308;
												#elif defined TTYD_JP
												uint32_t offset = 0x304;
												#elif defined TTYD_EU
												uint32_t offset = 0x30C;
												#endif
												
												MenuSecondaryValue = *reinterpret_cast<int32_t *>(
													ActorAddress + offset);
												break;
											}
											default:
											{
												break;
											}
										}
										break;
									}
								}
								break;
							}
							default:
							{	
								break;
							}
						}
						break;
					}
					case B:
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								// Go back to the previous menu
								resetMenu();
								CurrentMenuOption = enterPreviousMenu();
								break;
							}
							default:
							{
								closeSecondaryMenu();
								break;
							}
						}
						break;
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		case BATTLES_STATUSES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				switch (CurrentButton)
				{
					case DPADDOWN:
					case DPADUP:
					{
						if (tempSelectedOption == 0)
						{
							adjustBattlesStatusSelection(CurrentButton);
						}
						break;
					}
					case A:
					{
						if (tempSelectedOption == 0)
						{
							uint32_t ActorAddress = reinterpret_cast<uint32_t>(
								getActorPointer(tempMenuSelectedOption));
							
							if (ActorAddress == 0)
							{
								break;
							}
							
							uint32_t Counter = 0;
							if (tempCurrentMenuOption >= 20) // Explosion turns left
							{
								Counter += 3;
							}
							else if (tempCurrentMenuOption >= 18) // Can't use items
							{
								Counter += 2;
							}
							else if (tempCurrentMenuOption >= 1) // Sleep flags
							{
								Counter++;
							}
							
							uint32_t TotalOptions = BattlesStatusesLinesSize;
							uint32_t OffsetToLoad = 0x118 + tempCurrentMenuOption + Counter;
							
							if (tempCurrentMenuOption == (TotalOptions - 1))
							{
								// Currently modifying the defeated flag
								// Toggle the value
								bool *AddressToChange = reinterpret_cast<bool *>(
									ActorAddress + OffsetToLoad);
								
								*AddressToChange = !*AddressToChange;
							}
							else
							{
								SelectedOption = tempCurrentMenuOption + 1;
								MenuSelectionStates = tempCurrentMenuOption + 1;
								SecondaryMenuOption = getHighestAdjustableValueDigit(tempCurrentMenu) - 1;
								
								MenuSecondaryValue = *reinterpret_cast<int8_t *>(
									ActorAddress + OffsetToLoad);
							}
						}
						break;
					}
					case B:
					{
						switch (tempSelectedOption)
						{
							case 0:
							{
								// Go back to the previous menu
								resetMenu();
								CurrentMenuOption = enterPreviousMenu();
								break;
							}
							default:
							{
								closeSecondaryMenu();
								break;
							}
						}
						break;
					}
					default:
					{
						break;
					}
				}
			}
			break;
		}
		case DISPLAYS:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					// Enter the next menu
					uint32_t MenuToEnter;
					switch (tempCurrentMenuOption)
					{
						case ONSCREEN_TIMER:
						{
							MenuToEnter = DISPLAYS_ONSCREEN_TIMER;
							break;
						}
						default:
						{
							MenuToEnter = DISPLAYS_NO_BUTTON_COMBO;
							break;
						}
					}
					
					MenuSelectedOption = tempCurrentMenuOption;
					enterNextMenu(MenuToEnter, tempCurrentMenuOption);
					resetMenu();
					break;
				}
				case B:
				{
					// Go back to the previous menu
					resetMenu();
					CurrentMenuOption = enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case DISPLAYS_ONSCREEN_TIMER:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					adjustMenuNoPageEdit(CurrentButton);
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case ONSCREEN_TIMER_TURN_ON_OR_OFF:
								{
									// Flip the bool for the current cheat
									bool DisplayActive = Displays[tempMenuSelectedOption];
									Displays[tempMenuSelectedOption] = !DisplayActive;
									break;
								}
								case CHANGE_START_PAUSE_RESUME_BUTTON_COMBO:
								case CHANGE_RESET_BUTTON_COMBO:
								{
									ChangingCheatButtonCombo 	= true;
									Timer 						= secondsToFrames(3);
									SelectedOption 				= CurrentMenuOptionCheck;
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							// Currently changing a cheat button combo, so do nothing
							break;
						}
					}
					break;
				}
				case B:
				{
					if ((tempSelectedOption >= CHANGE_START_PAUSE_RESUME_BUTTON_COMBO) && 
						(tempSelectedOption <= CHANGE_RESET_BUTTON_COMBO))
					{
						// The work for this is done in drawChangeButtonCombo
						break;
					}
					else
					{
						// Go back to the previous menu
						MenuSelectedOption = 0;
						CurrentMenuOption = enterPreviousMenu();
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case DISPLAYS_NO_BUTTON_COMBO:
		{
			switch (CurrentButton)
			{
				case A:
				{
					// Flip the bool for the current display
					bool DisplayActive = Displays[tempMenuSelectedOption];
					Displays[tempMenuSelectedOption] = !DisplayActive;
					break;
				}
				case B:
				{
					// Go back to the previous menu
					MenuSelectedOption = 0;
					CurrentMenuOption = enterPreviousMenu();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case WARPS:
		{
			switch (CurrentButton)
			{
				case DPADLEFT:
				case DPADRIGHT:
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							adjustMenuNoPageEdit(CurrentButton);
							break;
						}
						case SELECT_WARP:
						{
							adjustWarpsSelection(CurrentButton);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
							switch (CurrentMenuOptionCheck)
							{
								case SELECT_WARP:
								{
									CurrentMenuOption 	= 0;
									Timer 				= 0;
									SelectedOption 		= CurrentMenuOptionCheck;
									break;
								}
								case WARP_BY_INDEX:
								{
									// Enter the next menu
									enterNextMenu(WARPS_INDEX, tempCurrentMenuOption);
									resetMenu();
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							switch (tempCurrentMenuOption)
							{
								case PIT_OF_100_TRIALS:
								{
									switch (tempMenuSelectedOption)
									{
										case 0:
										{
											if (checkIfInGame())
											{
												MenuSelectedOption = SELECTING_VALUE;
												MenuSecondaryValue = getCurrentPitFloor();
												
												SecondaryMenuOption = getHighestAdjustableValueDigit(
													tempCurrentMenu) - 1;
												
												Timer = 0;
											}
											else
											{
												FunctionReturnCode 	= NOT_IN_GAME;
												Timer 				= secondsToFrames(3);
											}
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
								default:
								{
									// Warp to the currently selected map and close the menu
									int32_t ReturnCode = warpToMap(tempCurrentMenuOption);
									switch (ReturnCode)
									{
										case UNKNOWN_BEHAVIOR:
										{
											break;
										}
										case SUCCESS:
										{
											closeMenu();
											return;
										}
										case NOT_IN_GAME:
										{
											FunctionReturnCode 	= ReturnCode;
											Timer 				= secondsToFrames(3);
											break;
										}
										default:
										{
											break;
										}
									}
									break;
								}
							}
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempSelectedOption)
					{
						case 0:
						{
							// Go back to the root
							resetMenu();
							CurrentMenuOption = enterPreviousMenu();
							break;
						}
						default:
						{
							if (tempMenuSelectedOption == 0)
							{
								closeSecondaryMenu();
							}
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case WARPS_INDEX:
		{
			switch (CurrentButton)
			{
				case DPADDOWN:
				case DPADUP:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							if (tempSelectedOption == 0)
							{
								adjustMenuNoPageEdit(CurrentButton);
							}
							break;
						}
						default:
						{
							adjustIndexWarpCurrentMapEntrancesPage(CurrentButton);
							break;
						}
					}
					break;
				}
				case A:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									uint32_t CurrentMenuOptionCheck = tempCurrentMenuOption + 1;
									switch (CurrentMenuOptionCheck)
									{
										case INDEX_VIEW_CURRENT_MAP_ENTRANCES:
										{
											MenuSelectedOption = CurrentMenuOptionCheck;
											break;
										}
										case INDEX_WARP_NOW:
										{
											int32_t ReturnCode = warpToMapByString(
												getMapFromIndex(static_cast<int32_t>(WarpByIndex.MapId)));
											
											switch (ReturnCode)
											{
												case SUCCESS:
												{
													// Set the flag for the loading zone to be adjusted
													WarpByIndex.RunIndexWarpCode = true;
													
													closeMenu();
													return;
												}
												case NOT_IN_GAME:
												{
													FunctionReturnCode 	= ReturnCode;
													Timer 				= secondsToFrames(3);
													break;
												}
												default:
												{
													break;
												}
											}
											break;
										}
										default:
										{
											
											SecondaryMenuOption = getHighestAdjustableValueDigit(
												tempCurrentMenu) - 1;
											
											Timer = 0;
											SelectedOption = CurrentMenuOptionCheck;
											
											if (CurrentMenuOptionCheck == INDEX_SELECT_MAP)
											{
												int32_t MapIndex = getMapIndex();
												if (MapIndex < 0)
												{
													MenuSecondaryValue = 0;
												}
												else
												{
													MenuSecondaryValue = MapIndex;
												}
											}
											else
											{
												MenuSecondaryValue = static_cast<int32_t>(WarpByIndex.EntranceId);
											}
											break;
										}
									}
									break;
								}
								default:
								{
									break;
								}
							}
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case B:
				{
					switch (tempMenuSelectedOption)
					{
						case 0:
						{
							switch (tempSelectedOption)
							{
								case 0:
								{
									// Go back to the root
									resetMenu();
									CurrentMenuOption = enterPreviousMenu();
									break;
								}
								default:
								{
									closeSecondaryMenu();
									break;
								}
							}
							break;
						}
						default:
						{
							MenuSelectedOption = 0;
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void drawMenu()
{
	// Display the window for the text to go in
	if (!HideMenu)
	{
		drawMenuWindow();
	}
	
	uint32_t tempCurrentMenu 			= CurrentMenu;
	// uint32_t tempCurrentMenuOption 	= CurrentMenuOption;
	uint32_t tempSelectedOption 		= SelectedOption;
	uint32_t tempMenuSelectionStates 	= MenuSelectionStates;
	// uint32_t tempSecondaryMenuOption = SecondaryMenuOption;
	// int32_t tempMenuSecondaryValue 	= MenuSecondaryValue;
	uint32_t tempMenuSelectedOption 	= MenuSelectedOption;
	uint32_t tempCurrentPage 			= CurrentPage;
	int32_t tempFunctionReturnCode 		= FunctionReturnCode;
	// uint32_t tempColumnSplitAmount 	= Menu[tempCurrentMenu].ColumnSplitAmount;
	// uint32_t tempTotalMenuOptions 	= Menu[tempCurrentMenu].TotalMenuOptions;
	
	// Display the text for the current page
	switch (tempCurrentMenu)
	{
		case ROOT:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the version number
			int32_t PosX = 150;
			int32_t PosY = 180;
			drawVersionNumber(PosX, PosY);
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case INVENTORY_MAIN:
		{
			uint32_t MaxOptionsPerPage = 20;
			correctInventoryCurrentMenuOptionAndPage(MaxOptionsPerPage);
			
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the item icons and text
			drawInventoryIconAndTextColumns();
			
			// Draw the appropriate display for the selected option
			switch (tempSelectedOption)
			{
				case ADD_BY_ID:
				{
					drawAddById(tempMenuSelectedOption);
					break;
				}
				case ADD_BY_ICON:
				{
					drawAddByIcon(tempMenuSelectedOption);
					break;
				}
				case DUPLICATE:
				case DELETE:
				{
					switch (tempFunctionReturnCode)
					{
						case 0:
						{
							if (getTotalItems() <= 0)
							{
								// Inventory just became empty, so exit the current menu
								closeSecondaryMenu();
							}
							break;
						}
						case INVENTORY_FULL:
						{
							// Inventory is currently full
							const char *CurrentLine = "The inventory is currently full.";
							drawErrorMessage(CurrentLine);
							break;
						}
						case INVENTORY_EMPTY:
						{
							// Inventory is currently empty
							const char *CurrentLine = "The inventory is currently empty.";
							drawErrorMessage(CurrentLine);
							break;
						}
						default:
						{
							break;
						}
					}
					break;
				}
				case CHANGE_BY_ID:
				case CHANGE_BY_ICON:
				{
					switch (tempFunctionReturnCode)
					{
						case 0:
						{
							if (getTotalItems() <= 0)
							{
								// Inventory just became empty, so exit the current menu
								tempMenuSelectionStates 	= 0;
								MenuSelectionStates 		= tempMenuSelectionStates;
								closeSecondaryMenu();
							}
							break;
						}
						case INVENTORY_EMPTY:
						{
							// Inventory is currently empty
							const char *CurrentLine = "The inventory is currently empty.";
							drawErrorMessage(CurrentLine);
							break;
						}
						default:
						{
							break;
						}
					}
					
					if (tempMenuSelectionStates != 0)
					{
						switch (tempSelectedOption)
						{
							case CHANGE_BY_ID:
							{
								drawAdjustableValue(true, tempMenuSelectedOption);
								break;
							}
							case CHANGE_BY_ICON:
							{
								drawAddByIconMain(tempMenuSelectedOption);
								break;
							}
							default:
							{
								break;
							}
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case CHEATS:
		{
			// Draw the text for the options
			uint32_t MaxOptionsPerPage = 18;
			drawSingleColumn(MaxOptionsPerPage, tempCurrentPage, false);
			
			// Draw the page number
			int32_t PosX = 150;
			int32_t PosY = 180;
			drawPageNumber(PosX, PosY, tempCurrentPage);
			break;
		}
		case CHEATS_CHANGE_SEQUENCE:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the text for the Sequence
			drawCheatsChangeSequence();
			
			if (tempMenuSelectionStates == CHANGE_SEQUENCE_VALUE)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			break;
		}
		case CHEATS_STANDARD:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the bool
			int32_t PosY = 120;
			drawCheatsBool(PosY);
			
			// Draw the button combo
			uint16_t *CurrentButtonCombo = &Cheat[tempMenuSelectedOption].ButtonCombo;
			const char *Description = "Button Combo (Can be pressed in any order)";
			PosY -= 60;
			drawButtonCombo(*CurrentButtonCombo, PosY, Description);
			
			if (tempSelectedOption == CHANGE_BUTTON_COMBO)
			{
				drawChangeButtonCombo(*CurrentButtonCombo);
			}
			break;
		}
		case CHEATS_NO_BUTTON_COMBO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			int32_t PosY = 140;
			drawCheatsBool(PosY);
			break;
		}
		case CHEATS_NPC_FORCE_DROP:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			int32_t PosY = 120;
			drawCheatsBool(PosY);
			
			// Draw the current item
			drawCheatsForcedDropItem();
			
			if (tempMenuSelectionStates == ITEM_DROP_CHANGE_BY_ID)
			{
				drawAdjustableValue(true, tempCurrentMenu);
			}
			break;
		}
		case CHEATS_MANAGE_FLAGS_MAIN:
		{
			// Draw the text for the options
			drawCheatsManageFlagsMain(tempMenuSelectionStates);
			
			if (tempSelectedOption > 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			break;
		}
		case CHEATS_CLEAR_AREA_FLAGS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the options for which area flags to clear
			drawCheatsClearArea();
			
			// Draw the window asking for confirmation to clear the flags
			if (tempSelectedOption == CLEAR_FLAGS)
			{
				const char *ConfirmationMessage = "This will clear all of the GSWF's for the\nselected area when the area is reloaded.\nAre you sure you want to do this?";
				drawConfirmationWindow(ConfirmationMessage);
			}
			break;
		}
		case STATS_MARIO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw each of Mario's stats
			drawMarioStats();
			
			switch (tempMenuSelectedOption)
			{
				case 0:
				{
					break;
				}
				case SPECIAL_MOVES:
				{
					drawMarioSpecialMovesOptions();
					break;
				}
				default:
				{
					drawAdjustableValue(false, tempCurrentMenu);
					break;
				}
			}
			break;
		}
		case STATS_PARTNERS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw each option to choose from
			drawPartnerStats();
			
			if (tempMenuSelectedOption == STATS_PARTNER_DISPLAY_YOSHI_COLORS)
			{
				drawPartnerChangeYoshiColorOptions();
			}
			else if (tempMenuSelectedOption != 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			
			// Draw the error message if the player tried to spawn a partner while either not in the game or in a battle
			if (tempFunctionReturnCode < 0)
			{
				const uint32_t YoshiPartner = 4;
				uint32_t CurrentPartner = getSelectedOptionPartnerValue();
				
				uint32_t AdditionalOptions = 0;
				if (CurrentPartner == YoshiPartner)
				{
					// Add an extra line for Yoshi
					AdditionalOptions++;
				}
				
				uint32_t FirstFreeSlot = TOGGLE; // enum index starts at 1
				if (CurrentMenuOption == (FirstFreeSlot + AdditionalOptions))
				{
					drawPartnerErrorMessage();
				}
			}
			break;
		}
		case STATS_FOLLOWERS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the text for the current follower out
			drawCurrentFollowerOut();
			
			if (tempSelectedOption != 0)
			{
				drawFollowersOptions();
			}
			
			// Draw the error message if the player tried to spawn a partner while either not in the game or in a battle
			if ((((CurrentMenuOption + 1) == BRING_OUT_FOLLOWER) || 
				(tempSelectedOption == BRING_OUT_FOLLOWER)) && 
					(tempFunctionReturnCode < 0))
			{
				drawFollowersErrorMessage();
			}
			break;
		}
		case SETTINGS:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			if (Timer > 0)
			{
				drawSettingsCurrentWork();
			}
			break;
		}
		case MEMORY:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the memory watches
			drawMemoryWatches();
			
			switch (tempFunctionReturnCode)
			{
				case ALL_SLOTS_EMPTY:
				{
					const char *CurrentLine = "All slots are currently empty.";
					drawErrorMessage(CurrentLine);
					break;
				}
				case NO_SLOTS_LEFT:
				{
					const char *CurrentLine = "There are no more free slots.";
					drawErrorMessage(CurrentLine);
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_MODIFY:
		{
			// Draw the main text
			// Don't draw if currently changing display positions
			if (!HideMenu)
			{
				drawMemoryModifyList();
			}
			
			switch (tempSelectedOption)
			{
				case CHANGE_TYPE:
				{
					drawMemoryTypeList();
					break;
				}
				case CHANGE_WATCH_POSITION:
				{
					drawMemoryChangeWatchPosition();
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case MEMORY_CHANGE_ADDRESS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the address, pointer levels, and final value
			drawMemoryChangeAddressList();
			
			if (tempMenuSelectionStates != 0)
			{
				drawMemoryWatchAdjustableValue(tempCurrentMenu);
			}
			break;
		}
		case BATTLES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text for the options
				drawSingleColumnMain();
				
				// Draw the page number
				int32_t PosX = 150;
				int32_t PosY = 180;
				drawPageNumber(PosX, PosY, tempCurrentPage);
				
				// Draw each actor
				drawBattlesActorsList();
			}
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case BATTLES_CURRENT_ACTOR:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text for the options
				drawSingleColumnSelectedOption();
				
				// Draw the HP/FP values
				drawBattlesActorStats();
				
				// Draw the current held item
				drawBattlesActorsHeldItem();
				
				if (tempSelectedOption > 0)
				{
					bool ChangingItem;
					if (tempSelectedOption == CHANGE_HELD_ITEM)
					{
						ChangingItem = true;
					}
					else
					{
						ChangingItem = false;
					}
					
					drawAdjustableValue(ChangingItem, tempCurrentMenu);
				}
			}
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case BATTLES_STATUSES:
		{
			// Close the menu if not in a battle
			if (!getBattleWorkPointer())
			{
				resetMenuToRoot();
				FunctionReturnCode = NOT_IN_BATTLE;
				Timer = secondsToFrames(3);
			}
			else
			{
				// Draw the text and icons
				drawBattlesStatusesList();
				
				// Draw the page number
				int32_t PosX = 150;
				int32_t PosY = 180;
				drawPageNumber(PosX, PosY, tempCurrentPage);
				
				if (tempSelectedOption > 0)
				{
					drawAdjustableValue(false, tempCurrentMenu);
				}
			}
			
			// Draw the error message if the player tried to use the battle menu while not in a battle
			if (tempFunctionReturnCode == NOT_IN_BATTLE)
			{
				drawNotInBattleErrorMessage();
			}
			break;
		}
		case DISPLAYS_ONSCREEN_TIMER:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the bool
			bool CurrentDisplay = Displays[tempMenuSelectedOption];
			const char *CurrentLine = DisplaysLines[tempMenuSelectedOption];
			
			int32_t PosY = 100;
			drawBoolOnOrOff(CurrentDisplay, CurrentLine, PosY);
			
			// Draw each button combo
			drawOnScreenTimerButtonCombos();
			
			if ((tempSelectedOption >= CHANGE_START_PAUSE_RESUME_BUTTON_COMBO) && 
				(tempSelectedOption <= CHANGE_RESET_BUTTON_COMBO))
			{
				drawChangeButtonCombo(OnScreenTimer.ButtonCombo[
					tempSelectedOption - OnScreenTimerOptionsSize]);
			}
			break;
		}
		case DISPLAYS_NO_BUTTON_COMBO:
		{
			// Draw the text for the options
			drawSingleColumnMain();
			
			// Draw the bool
			bool CurrentDisplay = Displays[tempMenuSelectedOption];
			const char *CurrentLine = DisplaysLines[tempMenuSelectedOption];
			int32_t PosY = 140;
			
			drawBoolOnOrOff(CurrentDisplay, CurrentLine, PosY);
			break;
		}
		case WARPS:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the warps options
			drawWarpsOptions();
			
			if (tempMenuSelectedOption == SELECTING_VALUE)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			
			// Draw the error message if the player tried to warp while either not in the game or in a battle
			if ((tempSelectedOption == SELECT_WARP) && 
				(tempFunctionReturnCode < 0))
			{
				drawWarpsErrorMessage();
			}
			break;
		}
		case WARPS_INDEX:
		{
			// Draw the text for the options
			drawSingleColumnSelectedOption();
			
			// Draw the map and entrance
			drawWarpIndexMapAndEntrance();
			
			if (tempSelectedOption > 0)
			{
				drawAdjustableValue(false, tempCurrentMenu);
			}
			else if (tempMenuSelectedOption > 0)
			{
				drawWarpIndexEntranceList();
			}
			
			// Draw the error message if the player tried to warp while either not in the game or in a battle
			if (tempFunctionReturnCode < 0)
			{
				drawWarpsErrorMessage();
			}
			break;
		}
		default:
		{
			drawSingleColumnMain();
			break;
		}
	}
}

void enableOrDisableMenu()
{
	// Check for user input
	uint16_t OpenMenuCombo = PAD_L | PAD_START;
	bool tempMenuIsDisplayed = MenuIsDisplayed;
	
	if (checkButtonCombo(OpenMenuCombo) && !PreventClosingMenu && !ChangingCheatButtonCombo)
	{
		resetMenuToRoot();
		tempMenuIsDisplayed = !tempMenuIsDisplayed;
		MenuIsDisplayed = tempMenuIsDisplayed;
		
		if (tempMenuIsDisplayed)
		{
			// Prevent the menu from checking inputs on the frame that it is opened
			FrameCounter = 1;
			
			// Raise the System Level if not in a battle
			raiseSystemLevel();
			
			// Disable the Pause Menu
			ttyd::win_main::winOpenDisable();
		}
		else
		{
			// Lower the System Level if not in a battle
			lowerSystemLevel();
			
			// Enable the Pause Menu
			ttyd::win_main::winOpenEnable();
		}
	}
	
	// Check if the menu should be displayed or not
	if (tempMenuIsDisplayed)
	{
		// Check for button inputs for the menu
		// Don't check any buttons if the frame counter is not 0
		// Don't check any buttons if currently changing button combos for cheats
		if ((FrameCounter == 0) && !ChangingCheatButtonCombo)
		{
			menuCheckButton();
		}
		
		// Display the menu
		drawFunctionOnDebugLayer(drawMenu);
	}
}

}