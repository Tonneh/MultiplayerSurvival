// Fill out your copyright notice in the Description page of Project Settings.

#include "HarvestingSystem/DestructableTree.h"

#include "Components/CapsuleComponent.h"

ADestructableTree::ADestructableTree()
{
}

void ADestructableTree::BeginPlay()
{
	Super::BeginPlay();
	AddForce();
}
