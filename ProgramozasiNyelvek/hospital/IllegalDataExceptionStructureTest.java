package hospital;

import static check.CheckThat.*;
import static check.CheckThat.Condition.*;
import org.junit.jupiter.api.*;
import check.CheckThat;

public class IllegalDataExceptionStructureTest {
    @BeforeAll
    public static void init() {
        CheckThat.theCheckedException("hospital.utils.IllegalDataException")
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL);
    }

    @Test
    public void constructor() {
        it.hasConstructor(withArgs("String"))
            .thatIs(VISIBLE_TO_ALL);
    }
}

