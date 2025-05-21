package hospital;

import static check.CheckThat.*;
import static check.CheckThat.Condition.*;
import org.junit.jupiter.api.*;
import check.CheckThat;

public class MedicinalStructureTest {
    @BeforeAll
    public static void init() {
        CheckThat.theInterface("hospital.people.Medicinal")
            .thatIs(VISIBLE_TO_ALL);
    }

    @Test
    public void methodAdmit() {
        it.hasMethod("admit", withParams("Patient"))
            .thatIs(NOT_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatReturnsNothing();
    }

    @Test
    public void methodTreat() {
        it.hasMethod("treat", withNoParams())
            .thatIs(NOT_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatReturnsNothing();
    }

    @Test
    public void methodDischarge() {
        it.hasMethod("discharge", withParams("String"))
            .thatIs(NOT_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .thatReturns("Patient");
    }
}

