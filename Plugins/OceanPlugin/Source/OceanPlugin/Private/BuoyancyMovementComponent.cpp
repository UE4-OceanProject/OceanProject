// OceanPlugin version

#include "OceanPluginPrivatePCH.h"
#include "BuoyancyMovementComponent.h"



UBuoyancyMovementComponent::UBuoyancyMovementComponent(const class FObjectInitializer& PCIP)
	: Super(PCIP) {
	Buoyancy = 1.0f;
	DirectionalDamping = FVector(0.2f, 2, 5);
	TestPointVolumeRadius = 10;
	//IsGeneratingWake = true;
	//WakeSize = 25.0f;
	}

void UBuoyancyMovementComponent::InitializeComponent() {
	Super::InitializeComponent();
	//UActorComponent::RegisterComponent();

	// If no OceanManager is defined auto-detect
	if (OceanManager == nullptr) {
		for (TActorIterator<AOceanManager> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			OceanManager = Cast<AOceanManager>(*ActorItr);
			break;
			}
		}

	_anchoredLocation = UpdatedComponent->GetComponentLocation();

	_baseLinearDamping = UpdatedPrimitive->GetLinearDamping();
	_baseAngularDamping = UpdatedPrimitive->GetAngularDamping();

	// TODO: Switch from using -GetGravityZ() to using actual buoyant force calculation
	_specificGravity = 0.99955;

	float r = TestPointVolumeRadius;

	float v = (r * (4 * PI * r * r)) / 3;

	_buoyantForce = _specificGravity * (v * 0.5);
	}

void UBuoyancyMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {

	if (OceanManager == nullptr)
		return;

	if (!UpdatedComponent->IsSimulatingPhysics()) {
		// Physics not simulated, snap to water surface
		FVector waveHeight = OceanManager->GetWaveHeightValue(_anchoredLocation, GetWorld()->GetTimeSeconds());
		UpdatedComponent->SetWorldLocation(FVector(_anchoredLocation.X + waveHeight.X, _anchoredLocation.Y + waveHeight.Y, waveHeight.Z), true);

		return;
		}

	FTransform transform = UpdatedComponent->GetComponentTransform();

	float testPointCount = TestPoints.Num();

	_pointsInWater = false;
	int numberOfPointsInWater = 0;
	for (int pointIndex = 0; pointIndex < testPointCount; pointIndex++) {
		FVector testPoint = TestPoints[pointIndex];

		FVector worldTestPoint = transform.TransformPosition(testPoint);

		FVector waveHeight = OceanManager->GetWaveHeightValue(worldTestPoint, GetWorld()->GetTimeSeconds());

		float delta = fabs(worldTestPoint.Z - waveHeight.Z) / TestPointVolumeRadius;

		float force = 0;
		if (worldTestPoint.Z > waveHeight.Z) {
			// Point above water apply gravity
			force = GetGravityZ();
			}
		else if (worldTestPoint.Z < waveHeight.Z) {
			// Point in water
			_pointsInWater = true;
			numberOfPointsInWater++;

			force = Buoyancy * -GetGravityZ();

			//UpdatedComponent->AddImpulseAtLocation(FVector(0, 0, _buoyantForce), worldTestPoint);
			}

		if (delta >= 1) {
			force = force / testPointCount;
			}
		else {
			force = (force * delta * delta * delta) / testPointCount;
			}

		UpdatedPrimitive->AddImpulseAtLocation(FVector(0, 0, force), worldTestPoint);

		}

	UpdatedPrimitive->SetEnableGravity(!_pointsInWater);

	if (_pointsInWater) {
		// Apply extra damping
		UpdatedPrimitive->SetLinearDamping(_baseLinearDamping + 0.01f * numberOfPointsInWater);
		UpdatedPrimitive->SetAngularDamping(_baseAngularDamping + 0.5f * numberOfPointsInWater);

		//if (IsGeneratingWake)
		//	{
		//	OceanManager->AddWake(FWakeStruct(transform.GetLocation(), transform.GetUnitAxis(EAxis::X), UpdatedComponent->GetPhysicsLinearVelocity(), WakeSize, GetWorld()->GetTimeSeconds()));
		//	}
		}

	/*if (location.Z > 0)
	UpdatedComponent->AddImpulse(FVector(0,0,GetGravityZ()));
	else if (location.Z < 0)
	UpdatedComponent->AddImpulse(FVector(0, 0, -GetGravityZ()));*/

	//Apply Directional Damping
	FVector rotatedDamping = UpdatedComponent->GetComponentRotation().RotateVector(DirectionalDamping).GetAbs();
	UpdatedPrimitive->AddImpulse(-UpdatedPrimitive->GetPhysicsLinearVelocity() * rotatedDamping);
	}

bool UBuoyancyMovementComponent::IsInWater() const {
	return _pointsInWater;
	}

void UBuoyancyMovementComponent::SetAnchoredLocation(FVector location) {
	_anchoredLocation = location;
	}
