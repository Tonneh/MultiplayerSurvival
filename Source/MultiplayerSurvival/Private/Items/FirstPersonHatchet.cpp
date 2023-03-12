// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/FirstPersonHatchet.h"

AFirstPersonHatchet::AFirstPersonHatchet()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
