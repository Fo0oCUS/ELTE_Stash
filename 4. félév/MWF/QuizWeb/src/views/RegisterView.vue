<script setup lang="ts">
import { ref } from "vue";
import { useRouter } from "vue-router";
import { userService } from "../services/userService";
import type { RegisterRequestDto } from "../Models/requestDtos";

const router = useRouter();

const form = ref<RegisterRequestDto>({
  email: "",
  password: "",
  name: ""
});

const confirmPassword = ref("");

const error = ref("");
const isLoading = ref(false);

const handleRegister = async () => {
  error.value = "";

  if (!form.value.email || !form.value.password || !form.value.name) {
    error.value = "All fields are required";
    return;
  }

  if (form.value.password !== confirmPassword.value) {
    error.value = "Passwords do not match";
    return;
  }

  isLoading.value = true;

  try {
    await userService.register(form.value);

    // 👉 siker után redirect loginra
    router.push("/login");
  } catch (e: any) {
    error.value = e.message;
    console.error(e);
  } finally {
    isLoading.value = false;
  }
};
</script>

<template>
  <div class="container py-5">
    <div class="row justify-content-center">
      <div class="col-12 col-sm-10 col-md-6 col-lg-5 col-xl-4">
        <div class="card border-0 shadow-sm rounded-4">
          <div class="card-body p-4 p-lg-5">
            <div class="text-center mb-4">
              <h3 class="fw-bold mb-1">Register</h3>
              <p class="text-muted mb-0">Create your account</p>
            </div>

            <div class="mb-3">
              <label class="form-label fw-semibold">Name</label>
              <input
                v-model="form.name"
                class="form-control form-control-lg"
                placeholder="Your name"
              />
            </div>

            <div class="mb-3">
              <label class="form-label fw-semibold">Email</label>
              <input
                v-model="form.email"
                type="email"
                class="form-control form-control-lg"
                placeholder="Email address"
              />
            </div>

            <div class="mb-3">
              <label class="form-label fw-semibold">Password</label>
              <input
                v-model="form.password"
                type="password"
                class="form-control form-control-lg"
                placeholder="Password"
              />
            </div>

            <div class="mb-4">
              <label class="form-label fw-semibold">Confirm Password</label>
              <input
                v-model="confirmPassword"
                type="password"
                class="form-control form-control-lg"
                placeholder="Confirm password"
              />
            </div>

            <button
              class="btn btn-primary btn-lg rounded-pill w-100"
              @click="handleRegister"
              :disabled="isLoading"
            >
              {{ isLoading ? "Registering..." : "Register" }}
            </button>

            <div v-if="error" class="alert alert-danger rounded-3 mt-4 text-center mb-0">
              {{ error }}
            </div>

            <div class="text-center mt-4">
              <small class="text-muted">
                Already have an account?
                <RouterLink to="/login" class="fw-semibold ms-1">Login</RouterLink>
              </small>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>