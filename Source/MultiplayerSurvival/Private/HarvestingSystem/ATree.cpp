// Fill out your copyright notice in the Description page of Project Settings.

#include "HarvestingSystem/ATree.h"

ATree::ATree()
{
	GetStaticMesh()->SetCollisionObjectType(ECC_GameTraceChannel2);
}
