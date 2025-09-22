package hospital;

import static check.CheckThat.*;
import static check.CheckThat.Condition.*;
import org.junit.jupiter.api.*;
import check.CheckThat;

public class DoctorStructureTest {
    @BeforeAll
    public static void init() {
        CheckThat.theClass("hospital.people.Doctor", withParent("hospital.people.Patient"), withInterfaces("hospital.people.Medicinal"))
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatHas(TEXTUAL_REPRESENTATION);
    }

    @Test
    public void fieldPatients() {
        it.hasField("patients: LinkedList of Patient")
            .thatIs(INSTANCE_LEVEL, MODIFIABLE, VISIBLE_TO_NONE)
            .thatHasNo(GETTER, SETTER);
    }

    @Test
    public void constructor() {
        it.hasConstructor(withArgs("int", "String", "int", "hospital.utils.Insurance"))
            .thatIs(VISIBLE_TO_ALL);
    }

    @Test
    public void methodWaitingList() {
        it.hasMethod("waitingList", withNoParams())
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatReturns("int");
    }

    @Test
    public void methodAdmit() {
        it.hasMethod("admit", withParams("Patient"))
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatReturnsNothing();
    }

    @Test
    public void methodTreat() {
        it.hasMethod("treat", withNoParams())
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatReturnsNothing();
    }

    @Test
    public void methodDischarge() {
        it.hasMethod("discharge", withParams("String"))
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatReturns("Patient");
    }
}

