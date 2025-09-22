package hospital;

import static check.CheckThat.*;
import static check.CheckThat.Condition.*;
import org.junit.jupiter.api.*;
import check.CheckThat;

public class PatientStructureTest {
    @BeforeAll
    public static void init() {
        CheckThat.theClass("hospital.people.Patient", withParent("hospital.people.Person"))
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatHas(TEXTUAL_REPRESENTATION)
            .thatHas(EQUALITY_CHECK)
            .thatHas(NATURAL_ORDERING);
    }

    @Test
    public void fieldPriority() {
        it.hasField("priority: int")
            .thatIs(INSTANCE_LEVEL, MODIFIABLE, VISIBLE_TO_SUBCLASSES)
            .thatHas(GETTER, SETTER);
    }

    @Test
    public void fieldAttendant() {
        it.hasField("attendant: String")
            .thatIs(INSTANCE_LEVEL, MODIFIABLE, VISIBLE_TO_SUBCLASSES)
            .thatHas(GETTER, SETTER);
    }

    @Test
    public void constructorForNormalPatient() {
        it.hasConstructor(withArgs("int", "String", "int", "hospital.utils.Insurance", "int", "String"))
            .thatIs(VISIBLE_TO_ALL);
    }

    @Test
    public void constructorForDoctorPatient() {
        it.hasConstructor(withArgs("int", "String", "int", "hospital.utils.Insurance"))
            .thatIs(VISIBLE_TO_ALL);
    }

}

