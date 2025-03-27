import React, { useState, useEffect, useCallback } from "react";
import axios from "axios";
import { useNavigate } from "react-router-dom";
import { Container, Typography, Button, Alert, Box, Grid } from "@mui/material";

function Dashboard() {
  const [balance, setBalance] = useState(0);
  const [message, setMessage] = useState("");
  const navigate = useNavigate();
  const accountId = localStorage.getItem("accountId");

  const fetchBalance = useCallback(async () => {
    try {
      const res = await axios.get(`http://localhost:3001/balance/${accountId}`);
      setBalance(res.data.balance);
    } catch (err) {
      setMessage("Error fetching balance");
    }
  }, [accountId]);

  useEffect(() => {
    if (!localStorage.getItem("token")) {
      navigate("/");
    } else {
      fetchBalance();
    }
  }, [fetchBalance, navigate]);

  return (
    <Container maxWidth="md" sx={{ mt: 4 }}>
      <Typography variant="h4" gutterBottom>
        Dashboard
      </Typography>
      <Typography variant="h6" gutterBottom>
        Your Balance: ${balance}
      </Typography>
      {message && (
        <Alert severity="info" sx={{ mb: 2 }}>
          {message}
        </Alert>
      )}
      <Box sx={{ mb: 3 }}>
        <Grid container spacing={2}>
          <Grid item xs={4}>
            <Button
              variant="contained"
              fullWidth
              onClick={() => navigate("/deposit")}
            >
              Deposit
            </Button>
          </Grid>
          <Grid item xs={4}>
            <Button
              variant="contained"
              fullWidth
              onClick={() => navigate("/withdraw")}
            >
              Withdraw
            </Button>
          </Grid>
          <Grid item xs={4}>
            <Button
              variant="contained"
              fullWidth
              onClick={() => navigate("/transfer")}
            >
              Transfer
            </Button>
          </Grid>
        </Grid>
      </Box>
      <Box sx={{ display: "flex", gap: 2, mb: 2 }}>
        <Button variant="outlined" onClick={() => navigate("/transactions")}>
          View Transactions
        </Button>
        <Button variant="outlined" onClick={() => navigate("/update-password")}>
          Update Password
        </Button>
      </Box>
    </Container>
  );
}

export default Dashboard;
