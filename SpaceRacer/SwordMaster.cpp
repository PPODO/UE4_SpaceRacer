#include "SwordMaster.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ConstructorHelpers.h"
#include "Engine/World.h"

static const uint32 MaxSpinSwordCount = 12;
static const float MaxSpinSpeed = 17.5f;
static const float MinSpinSpeed = 2.5f;

ASwordMaster::ASwordMaster() {
	m_RootComponent = CreateDefaultSubobject<USceneComponent>(L"Collision Component");
	RootComponent = m_RootComponent;

	for (uint32 i = 0; i < MaxSpinSwordCount; i++) {
		float CurrentAngle = FMath::DegreesToRadians(i * (360.f / float(MaxSpinSwordCount)));
		FString SwordMeshDirectory = L"StaticMesh'/Game/Swords/";
		FString MeshName = FString::Printf(L"Sword%d.Sword%d'", (i + 1), (i + 1));
		SwordMeshDirectory.Append(MeshName);

		UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(L"Sowrd%d Mesh", i + 1));
		::ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObject(*SwordMeshDirectory);
		if (MeshObject.Succeeded()) {
			MeshComponent->SetStaticMesh(MeshObject.Object);
		}
		MeshComponent->SetRelativeLocation(FVector(FMath::Cos(CurrentAngle) * m_fSwordDistance, FMath::Sin(CurrentAngle) * m_fSwordDistance, 100.f));
		MeshComponent->SetRelativeRotation(FRotator(0.f, 0.f, 180.f));
		MeshComponent->SetupAttachment(RootComponent);

		m_SwordMeshes.Add(MeshComponent);
	}

	::ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(L"ParticleSystem'/Game/ProjectilesPack/Particles/MicrodestructorMissile/P_Microdestructor_Swarm.P_Microdestructor_Swarm'");
	if (Particle.Succeeded()) {
		m_ActivateSwordMasterEffect = Particle.Object;
	}

	PrimaryActorTick.bCanEverTick = true;
}

void ASwordMaster::BeginPlay() {
	Super::BeginPlay();

	ActivateSwordMater(false, false);
}

void ASwordMaster::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if (IsActorTickEnabled()) {
		if (!FMath::IsNearlyEqual(m_fCurrentSpinSpeed, MinSpinSpeed)) {
			m_fCurrentSpinSpeed = UKismetMathLibrary::FInterpTo(m_fCurrentSpinSpeed, MinSpinSpeed, DeltaTime, m_fInterpSpeed);
		}

		m_RootComponent->AddRelativeRotation(FRotator(0.f, m_fCurrentSpinSpeed / 1.5f, 0.f));

		for (auto It : m_SwordMeshes) {
			It->AddRelativeRotation(FRotator(0.f, m_fCurrentSpinSpeed, 0.f));
		}
	}
}

void ASwordMaster::ActivateSwordMater(bool bActivate, bool bSpawnParticle) {
	if (bActivate) {
		m_fCurrentSpinSpeed = MaxSpinSpeed;
	}
	SetActorTickEnabled(bActivate);
	SetActorHiddenInGame(!bActivate);
	SetActorEnableCollision(bActivate);

	if (bSpawnParticle) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ActivateSwordMasterEffect, GetActorLocation());
	}
}